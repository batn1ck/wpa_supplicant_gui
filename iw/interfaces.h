#ifndef _NET_INTERFACES
#define _NET_INTERFACES

#define MAC_ADDR_LEN 6

#ifndef IFNAMSIZ
    #define IFNAMSIZ 16
#endif

struct iw_info {
    char iw_name[IFNAMSIZ];
    unsigned char mac_addr[MAC_ADDR_LEN];
    struct iw_info *iw_next;
};

struct iw_info *get_iw_info(void);
int change_iw_mac_addr(char *);
void clear_iw_list(struct iw_info *);

#endif
