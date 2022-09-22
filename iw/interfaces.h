#ifndef _NET_INTERFACES
#define _NET_INTERFACES

#define MAC_ADDR_LEN 6
#define MAC_ADDR_STR_LEN MAC_ADDR_LEN*2 + (MAC_ADDR_LEN-1) + 1
#define IPv4_N_OCTETS 4
#define IPv4_ADDR_STR_LEN 16

#ifndef IFNAMSIZ
    #define IFNAMSIZ 16
#endif

struct ipv4_settings {
    union {
        uint32_t address;
        char octets[IPv4_N_OCTETS];
    } ipv4_address;

    union {
        uint32_t netmask;
        char octets[IPv4_N_OCTETS];
    } ipv4_netmask;
};

struct iw_info {
    char iw_name[IFNAMSIZ];
    unsigned char mac_addr[MAC_ADDR_LEN];
    struct iw_info *iw_next;
};

int get_iw_ipv4_info(char *, struct ipv4_settings *);
struct iw_info *get_iw_info(void);
int change_iw_mac_addr(char *);
int set_iw_ipv4_addr(char *, struct ipv4_settings);
int get_iw_mac_addr(char *);
void clear_iw_list(struct iw_info *);

#endif
