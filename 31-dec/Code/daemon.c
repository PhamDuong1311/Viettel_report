#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define ARP_REQ 1
#define ARP_REP 2

void create_daemon(void);
void build_arp_request();
void build_arp_reply();
void arp_cache();
void ipc();
void get_mac_addr(const char *iface);

struct arp_header {
    uint16_t hw_type;
    uint16_t pro_type;
    uint8_t hw_length;
    uint8_t pro_length;
    uint16_t opcode;
    uint8_t sender_ip[4];
    uint8_t sender_mac[6];
    uint8_t target_ip[4];
    uint8_t target_mac[6];
};

int main() {
    const char *iface = "eth0";
    get_mac_addr(iface);
    return 0;
}

void create_daemon() {
    pid_t pid, sid;

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) {
        exit(0);
    }
    
    sid = setsid();

    if (sid < 0) {
        perror("setsid failed");
        exit(1);
    }

    if (chdir("/") < 0) {
        perror("chdir failed");
        exit(1);
    }

    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while(1) {
        sleep(1);
    }
}

void get_mac_addr(const char *iface) {
    struct ifreq ifr;
    int sock;
    uint8_t *mac;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("sock failed");
        exit(1);
    }

    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl failed");
        close(sock);
        exit(1);
    }

    mac = (uint8_t *)ifr.ifr_hwaddr.sa_data;
    printf("MAC address of %s: %02x:%02x:%02x:%02x:%02x:%02x\n", iface, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    close(sock);
}

void build_arp_request(uint8_t src_ip, uint) {

}

void build_arp_reply() {

}
