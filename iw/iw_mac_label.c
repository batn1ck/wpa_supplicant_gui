#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include "data_structure.h"
#include "interfaces.h"
#include "iw_mac_label.h"

extern char *selected_iw;

void iw_mac_label_show(GtkWidget *widget, GtkBuilder *builder)
{
    GtkWidget *mac_label;
    int fd;
    struct ifreq ifr;
    unsigned char mac_addr[MAC_ADDR_LEN];
    char mac_addr_str[MAC_ADDR_LEN*2 + (MAC_ADDR_LEN-1) + 3];

    if ( !selected_iw )
        return;

    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        return;

    if ( change_iw_mac_addr(selected_iw) != 0 )
        return;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, selected_iw, IFNAMSIZ);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    memcpy(mac_addr, (unsigned char *) ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
    snprintf(mac_addr_str, MAC_ADDR_LEN*2 + (MAC_ADDR_LEN-1) + 3, 
             "(%.2x:%.2x:%.2x:%.2x:%.2x:%.2x)", 
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]
    );

    mac_label = GTK_WIDGET(gtk_builder_get_object(builder, "mac_addr_label"));
    gtk_label_set_text(GTK_LABEL(mac_label), (const char *) mac_addr_str);
}
