#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include "interfaces.h"

char *selected_iw = NULL;

static bool check_wireless(const char *ifname, char *protocol)
{
    int sock = -1;
    struct iwreq pwrq;
    memset(&pwrq, 0, sizeof(pwrq));
    strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return false;
    }

    if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
        if (protocol) 
            strncpy(protocol, pwrq.u.name, IFNAMSIZ);
        
        close(sock);
        return true;
    }

    close(sock);
    return false;
}

static struct iw_info *add_iw_interface_info(struct iw_info *iw_list, const char *if_name)
{
    struct ifreq ifr;
    int fd;

    if ( !if_name )
        return NULL;

    if ( iw_list ) {
        iw_list->iw_next = (struct iw_info *) malloc(sizeof(struct iw_info));
        iw_list = iw_list->iw_next;
    } else
        iw_list = (struct iw_info *) malloc(sizeof(struct iw_info));
    
    if ( !iw_list )
        return NULL;

    snprintf(iw_list->iw_name, IFNAMSIZ, "%s", if_name);

    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        free(iw_list);
        iw_list = NULL;
        return iw_list;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    memcpy(iw_list->mac_addr, (unsigned char *) ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
    iw_list->iw_next = NULL;

    return iw_list;
}

struct iw_info *get_iw_info(void)
{
    struct ifaddrs *ifaddr, *ifa_pointer;
    struct iw_info *iw_names_list = NULL, *iw_last = NULL;

    if ( getifaddrs(&ifaddr) < 0 )
        return NULL;

    for (ifa_pointer = ifaddr; ifa_pointer != NULL; ifa_pointer = ifa_pointer->ifa_next) {
        char protocol[IFNAMSIZ] = {0};

        if ( ifa_pointer->ifa_addr == NULL ||
             ifa_pointer->ifa_addr->sa_family != AF_PACKET )
            continue;

        if ( check_wireless(ifa_pointer->ifa_name, protocol) ) {
            iw_last = add_iw_interface_info(iw_last, ifa_pointer->ifa_name);
            if ( !iw_last )
                continue;

            if ( !iw_names_list )
                iw_names_list = iw_last;
        }
    }

    freeifaddrs(ifaddr);
    return iw_names_list;
}

void clear_iw_list(struct iw_info *iw_list)
{
    if ( iw_list == NULL )
        return;

    struct iw_info *temp;

    while (iw_list != NULL ) {
        temp = iw_list;
        iw_list = iw_list->iw_next;
        free(temp);
    }
}

int get_iw_mac_addr(char *mac_addr_str)
{
    if ( !mac_addr_str || !selected_iw )
        return 1;

    struct ifreq ifr;
    unsigned char mac_addr[MAC_ADDR_LEN];
    int fd;

    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        return errno;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, selected_iw, IFNAMSIZ);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    memcpy(mac_addr, (unsigned char *) ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
    snprintf(mac_addr_str, MAC_ADDR_STR_LEN,
             "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", 
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]
    );

    return 0;
}

int change_iw_mac_addr(char *if_name)
{
    struct ifreq ifr;
    short backup_flags;
    int fd;

    if ( !if_name )
        return 1;

    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        return errno;

    srand(time(NULL));

    strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

    if ( ioctl(fd, SIOCGIFFLAGS, &ifr) < 0 )
        return errno;

    backup_flags = ifr.ifr_flags;
    ifr.ifr_flags &= ~IFF_UP;

    if ( ioctl(fd, SIOCSIFFLAGS, &ifr) < 0 )
        return errno;

    ifr.ifr_hwaddr.sa_data[0] = rand() % 256;
    ifr.ifr_hwaddr.sa_data[1] = rand() % 256;
    ifr.ifr_hwaddr.sa_data[2] = rand() % 256;
    ifr.ifr_hwaddr.sa_data[3] = rand() % 256;
    ifr.ifr_hwaddr.sa_data[4] = rand() % 256;
    ifr.ifr_hwaddr.sa_data[5] = rand() % 256;
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    if ( ioctl(fd, SIOCSIFHWADDR, &ifr) < 0 ) {
        fprintf(stderr, "ioctl: %s: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", strerror(errno),
    ifr.ifr_hwaddr.sa_data[0],
    ifr.ifr_hwaddr.sa_data[1],
    ifr.ifr_hwaddr.sa_data[2],
    ifr.ifr_hwaddr.sa_data[3],
    ifr.ifr_hwaddr.sa_data[4],
    ifr.ifr_hwaddr.sa_data[5]
                );
        return errno;
    }

    backup_flags |= IFF_UP;
    ifr.ifr_flags = backup_flags;

    if ( ioctl(fd, SIOCSIFFLAGS, &ifr) < 0 )
        return errno;

    return 0;
}
