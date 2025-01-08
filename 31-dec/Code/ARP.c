#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <time.h>

#define ARP_CACHE_TIMEOUT 15
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define MAX_ARP_CACHE_ENTRIES 100

typedef struct {
    unsigned char mac[6];
    unsigned char ip[4];
    time_t timestamp;
} arp_cache_entry;

arp_cache_entry arp_cache[MAX_ARP_CACHE_ENTRIES];

void check_arp_cache_timeout() {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_ARP_CACHE_ENTRIES; i++) {
        if (arp_cache[i].timestamp != 0 && now - arp_cache[i].timestamp >= ARP_CACHE_TIMEOUT) {
            printf("Removing expired ARP entry: IP: %d.%d.%d.%d\n",
                   arp_cache[i].ip[0], arp_cache[i].ip[1], arp_cache[i].ip[2], arp_cache[i].ip[3]);
            memset(&arp_cache[i], 0, sizeof(arp_cache_entry)); 
        }
    }
}

void add_arp_cache(unsigned char* ip, unsigned char* mac) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_ARP_CACHE_ENTRIES; i++) {
        if (memcmp(arp_cache[i].ip, ip, 4) == 0) {
            memcpy(arp_cache[i].mac, mac, 6);
            arp_cache[i].timestamp = now;
            return;
        }
    }
}

struct arp_hdr {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t hw_size;
    uint8_t proto_size;
    uint16_t opcode;
    unsigned char sender_mac[6];
    unsigned char sender_ip[4];
    unsigned char target_mac[6];
    unsigned char target_ip[4];
};

// Build ARP request packet
void build_arp_request(unsigned char* packet, unsigned char* src_mac, unsigned char* src_ip, unsigned char* target_ip) {
    struct ethhdr *eth = (struct ethhdr *) packet;
    struct arp_hdr *arp = (struct arp_hdr *)(packet + sizeof(struct ethhdr));

    memset(eth->h_dest, 0xFF, ETH_ALEN); 
    memcpy(eth->h_source, src_mac, ETH_ALEN);
    eth->h_proto = htons(ETH_P_ARP);

    arp->hw_type = htons(ARPHRD_ETHER);
    arp->proto_type = htons(ETH_P_IP);
    arp->hw_size = 6;
    arp->proto_size = 4;
    arp->opcode = htons(ARP_REQUEST);
    memcpy(arp->sender_mac, src_mac, 6);
    memcpy(arp->sender_ip, src_ip, 4);
    memset(arp->target_mac, 0x00, 6);
    memcpy(arp->target_ip, target_ip, 4);
}

// Build ARP reply packet
void build_arp_reply(unsigned char* packet, unsigned char* src_mac, unsigned char* src_ip, unsigned char* target_mac, unsigned char* target_ip) {
    struct ethhdr *eth = (struct ethhdr *) packet;
    struct arp_hdr *arp = (struct arp_hdr *)(packet + sizeof(struct ethhdr));

    memcpy(eth->h_dest, target_mac, ETH_ALEN);
    memcpy(eth->h_source, src_mac, ETH_ALEN);
    eth->h_proto = htons(ETH_P_ARP);

    arp->hw_type = htons(ARPHRD_ETHER);
    arp->proto_type = htons(ETH_P_IP);
    arp->hw_size = 6;
    arp->proto_size = 4;
    arp->opcode = htons(ARP_REPLY);
    memcpy(arp->sender_mac, src_mac, 6);
    memcpy(arp->sender_ip, src_ip, 4);
    memcpy(arp->target_mac, target_mac, 6);
    memcpy(arp->target_ip, target_ip, 4);
}

// Create raw socket
int create_raw_socket() {
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    return sockfd;
}

// Process incoming ARP packets
void process_arp_packet(int sockfd) {
    unsigned char buffer[65536];
    ssize_t data_size;

    while (1) {
        data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (data_size < 0) {
            perror("recvfrom failed");
            continue;
        }

        struct ethhdr *eth = (struct ethhdr*) buffer;
        struct arp_hdr *arp = (struct arp_hdr*) (buffer + sizeof(struct ethhdr));

        if (ntohs(eth->h_proto) == ETH_P_ARP) {
            if (ntohs(arp->opcode) == ARP_REQUEST) {
                printf("Received ARP Request\n");
                
                // Process ARP request: if we have the IP in ARP cache, send ARP reply
                unsigned char sender_ip[4];
                memcpy(sender_ip, arp->sender_ip, 4);

                for (int i = 0; i < MAX_ARP_CACHE_ENTRIES; i++) {
                    if (memcmp(arp_cache[i].ip, arp->target_ip, 4) == 0) {
                        // Send ARP reply
                        build_arp_reply(buffer, arp_cache[i].mac, arp_cache[i].ip, arp->sender_mac, arp->sender_ip);
                        struct sockaddr_ll sa;
                        memset(&sa, 0, sizeof(struct sockaddr_ll));
                        sa.sll_protocol = htons(ETH_P_ARP);
                        sendto(sockfd, buffer, sizeof(struct ethhdr) + sizeof(struct arp_hdr), 0, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll));
                        break;
                    }
                }
            } else if (ntohs(arp->opcode) == ARP_REPLY) {
                printf("Received ARP Reply\n");
                add_arp_cache(arp->sender_ip, arp->sender_mac);
            }
        }

        check_arp_cache_timeout();
    }
}

int main() {
    int sockfd = create_raw_socket();
    printf("Starting ARP handler...\n");
    process_arp_packet(sockfd);
    close(sockfd);
    return 0;
}
