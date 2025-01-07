#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/arp.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <time.h>

#define ARP_CACHE_TIMEOUT 15 // ARP Cache timeout (15 seconds)
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define MAX_ARP_CACHE_ENTRIES 100

// ARP cache entry structure
typedef struct {
    unsigned char mac[6];
    unsigned char ip[4];
    time_t timestamp;
} arp_cache_entry;

// ARP cache array
arp_cache_entry arp_cache[MAX_ARP_CACHE_ENTRIES];

// Clear ARP cache
void clear_arp_cache() {
    for (int i = 0; i < MAX_ARP_CACHE_ENTRIES; i++) {
        memset(&arp_cache[i], 0, sizeof(arp_cache_entry));
    }
}

// Check ARP cache for IP address
arp_cache_entry* lookup_arp_cache(unsigned char* ip) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_ARP_CACHE_ENTRIES; i++) {
        if (memcmp(arp_cache[i].ip, ip, 4) == 0) {
            if (now - arp_cache[i].timestamp < ARP_CACHE_TIMEOUT) {
                return &arp_cache[i];
            } else {
                // Entry has expired, clear it
                memset(&arp_cache[i], 0, sizeof(arp_cache_entry));
            }
        }
    }
    return NULL;
}

// Add entry to ARP cache
void add_arp_cache(unsigned char* ip, unsigned char* mac) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_ARP_CACHE_ENTRIES; i++) {
        if (arp_cache[i].mac[0] == 0) {
            memcpy(arp_cache[i].ip, ip, 4);
            memcpy(arp_cache[i].mac, mac, 6);
            arp_cache[i].timestamp = now;
            return;
        }
    }
}

// Build ARP request packet
void build_arp_request(unsigned char* packet, unsigned char* src_mac, unsigned char* src_ip, unsigned char* target_ip) {
    struct ethhdr *eth = (struct ethhdr *) packet;
    struct arp_hdr *arp = (struct arp_hdr *)(packet + sizeof(struct ethhdr));

    // Ethernet header
    memset(eth->h_dest, 0xFF, ETH_ALEN); // Broadcast address
    memcpy(eth->h_source, src_mac, ETH_ALEN);
    eth->h_proto = htons(ETH_P_ARP);

    // ARP packet
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

    // Ethernet header
    memcpy(eth->h_dest, target_mac, ETH_ALEN);
    memcpy(eth->h_source, src_mac, ETH_ALEN);
    eth->h_proto = htons(ETH_P_ARP);

    // ARP packet
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

// Main function to run ARP handler
int main() {
    int sock;
    struct sockaddr_ll sa;
    unsigned char packet[42 + 28]; // Ethernet + ARP
    unsigned char src_mac[6];
    unsigned char src_ip[4] = {192, 168, 1, 1}; // Example IP
    unsigned char target_ip[4] = {192, 168, 1, 2}; // Target IP

    // Create raw socket
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Get source MAC address
    struct ifreq ifr;
    strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name));
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
        perror("IOCTL failed to get MAC address");
        close(sock);
        exit(1);
    }
    memcpy(src_mac, ifr.ifr_hwaddr.sa_data, 6);

    // Build and send ARP request
    build_arp_request(packet, src_mac, src_ip, target_ip);
    memset(&sa, 0, sizeof(struct sockaddr_ll));
    sa.sll_protocol = htons(ETH_P_ARP);
    sa.sll_ifindex = if_nametoindex("eth0");

    // Send ARP request
    if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll)) < 0) {
        perror("Send ARP request failed");
        close(sock);
        exit(1);
    }

    printf("ARP request sent\n");

    // Close socket
    close(sock);
    return 0;
}
