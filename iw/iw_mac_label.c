#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include "interfaces.h"
#include "iw_mac_label.h"

extern char *selected_iw;

void iw_mac_label_show(GtkWidget *widget, GtkBuilder *builder)
{
    if ( !builder )
        return;

    GtkWidget *mac_label;
    int alpha_offset; 
    size_t i, mac_string_len;
    char mac_addr_str[MAC_ADDR_STR_LEN];

    if ( !selected_iw )
        return;

    if ( get_iw_mac_addr(mac_addr_str) != 0 )
        return;

    mac_string_len = strlen(mac_addr_str);
    alpha_offset = 'a' - 'A';

    for (i = 0; i < mac_string_len; i++)
        if ( mac_addr_str[i] >= 'a' && mac_addr_str[i] <= 'f' )
            mac_addr_str[i] -= alpha_offset;

    mac_label = GTK_WIDGET(gtk_builder_get_object(builder, "mac_addr_label"));
    gtk_label_set_text(GTK_LABEL(mac_label), (const char *) mac_addr_str);
    gtk_widget_show(mac_label);
}
