#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include "net_settings_window.h"
#include "iw/interfaces.h"
#include "gui_alert.h"

extern char *selected_iw;

static void set_ipv4_settings_sensitive(GtkBuilder *builder, gboolean status)
{
    if ( !builder )
        return;

    GtkWidget *ipv4_addr_widget, *ipv4_gw_widget, *ipv4_dns_widget, *ipv4_netmask_widget;
    unsigned int i;
    char addr_widget_name[WIDGET_NAME_LEN], gw_widget_name[WIDGET_NAME_LEN], dns_widget_name[WIDGET_NAME_LEN];

    for (i = 1; i < IPv4_WIDGETS_N+1; i++) {
        snprintf(addr_widget_name, WIDGET_NAME_LEN, "ipv4_address_entry_%u", i);
        snprintf(gw_widget_name, WIDGET_NAME_LEN, "ipv4_gateway_entry_%u", i);
        snprintf(dns_widget_name, WIDGET_NAME_LEN, "ipv4_dns_server_entry_%u", i);

        ipv4_addr_widget = GTK_WIDGET(gtk_builder_get_object(builder, addr_widget_name));
        ipv4_gw_widget = GTK_WIDGET(gtk_builder_get_object(builder, gw_widget_name));
        ipv4_dns_widget = GTK_WIDGET(gtk_builder_get_object(builder, dns_widget_name));

        gtk_widget_set_sensitive(ipv4_addr_widget, status);
        gtk_widget_set_sensitive(ipv4_gw_widget, status);
        gtk_widget_set_sensitive(ipv4_dns_widget, status);
    }

    ipv4_netmask_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_address_netmask_entry"));
    gtk_widget_set_sensitive(ipv4_netmask_widget, status);
}

static void set_ipv6_settings_sensitive(GtkBuilder *builder, gboolean status)
{
    if ( !builder )
        return;

    GtkWidget *ipv6_addr_widget, *ipv6_gw_widget, *ipv6_dns_widget, *ipv6_netmask_widget;
    
    ipv6_addr_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_address_entry"));
    ipv6_netmask_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_address_netmask_entry"));
    ipv6_gw_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_gateway_entry"));
    ipv6_dns_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_dns_server_entry"));

    gtk_widget_set_sensitive(ipv6_addr_widget, status);
    gtk_widget_set_sensitive(ipv6_netmask_widget, status);
    gtk_widget_set_sensitive(ipv6_gw_widget, status);
    gtk_widget_set_sensitive(ipv6_dns_widget, status);
}

static void enable_static_net_settings_sensitive(GtkToggleButton *tog_button, GtkBuilder *builder)
{
    gboolean toggled = gtk_toggle_button_get_active(tog_button);

    if ( !toggled )
        return;

    GtkWidget *ipv4_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_enable"));
    GtkWidget *ipv6_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_enable"));
    gboolean ipv4_toggle = gtk_toggle_button_get_active((GtkToggleButton *) ipv4_enable);
    gboolean ipv6_toggle = gtk_toggle_button_get_active((GtkToggleButton *) ipv6_enable);
    
    if ( ipv4_toggle )
        set_ipv4_settings_sensitive(builder, TRUE);

    if ( ipv6_toggle )
        set_ipv6_settings_sensitive(builder, TRUE);

    gtk_widget_set_sensitive(ipv4_enable, TRUE);
    gtk_widget_set_sensitive(ipv6_enable, TRUE);
}

static void disable_static_net_settings_sensitive(GtkToggleButton *tog_button, GtkBuilder *builder)
{
    gboolean toggled = gtk_toggle_button_get_active(tog_button);

    if ( !toggled )
        return;

    GtkWidget *ipv4_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_enable"));
    GtkWidget *ipv6_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_enable"));

    set_ipv4_settings_sensitive(builder, FALSE);
    set_ipv6_settings_sensitive(builder, FALSE);
    gtk_widget_set_sensitive(ipv4_enable, FALSE);
    gtk_widget_set_sensitive(ipv6_enable, FALSE);
}

static void enable_ipv4_settings(GtkToggleButton *tog_button, GtkBuilder *builder)
{
    gboolean status = gtk_toggle_button_get_active(tog_button);
    set_ipv4_settings_sensitive(builder, status);
}

static void enable_ipv6_settings(GtkToggleButton *tog_button, GtkBuilder *builder)
{
    gboolean status = gtk_toggle_button_get_active(tog_button);
    set_ipv6_settings_sensitive(builder, status);
}

static void set_ipv4_settings_to_entry(GtkBuilder *builder)
{
    struct ipv4_settings ipv4_info;
    char addr_widget_name[WIDGET_NAME_LEN], ipv4_addr_octet_i[4], netmask_str[3];
    unsigned int i, netmask_bits_n;
    uint32_t netmask;

    if ( !selected_iw )
        return;

    if ( get_iw_ipv4_info(selected_iw, &ipv4_info) != 0 )
        return;

    GtkWidget *ipv4_entry_widget,
              *netmask_entry_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_address_netmask_entry"));

    for (i = 0; i < IPv4_WIDGETS_N; i++) {
        snprintf(addr_widget_name, WIDGET_NAME_LEN, "ipv4_address_entry_%u", i+1);
        snprintf(ipv4_addr_octet_i, 4, "%hhu", ipv4_info.ipv4_address.octets[i]);

        ipv4_entry_widget = GTK_WIDGET(gtk_builder_get_object(builder, addr_widget_name));

        gtk_entry_set_text(GTK_ENTRY(ipv4_entry_widget), ipv4_addr_octet_i);
    }

    netmask = htonl(ipv4_info.ipv4_netmask.netmask);
    netmask_bits_n = 0;

    while ( netmask ) {
        netmask_bits_n++;
        netmask <<= 1;
    }

    snprintf(netmask_str, 3, "%u", netmask_bits_n);
    gtk_entry_set_text(GTK_ENTRY(netmask_entry_widget), netmask_str);
}

static int get_ipv4_settings_from_widgets(GtkBuilder *builder,
                             struct ipv4_settings *ip_addr_settings,
                             char *ip_gateway,
                             char *ip_dns)
{
    unsigned int i = 31;
    bool addr_set = true, gateway_set = true, dns_set = true;
    int addr_i, gateway_i, dns_i, netmask_bits;
    char addr_widget_name[WIDGET_NAME_LEN], gateway_widget_name[WIDGET_NAME_LEN], dns_widget_name[WIDGET_NAME_LEN];
    char *addr_entry_res, *gateway_entry_res, *dns_entry_res, *netmask_entry_res;
    GtkWidget *addr_entry_widget, *gateway_entry_widget, *dns_entry_widget, *netmask_entry_widget;

    if ( !builder )
        return -1;

    if ( !ip_addr_settings )
        return -2;

    if ( !ip_gateway )
        return -3;

    if ( !ip_dns )
        return -4;

    memset(ip_addr_settings, 0, sizeof(struct ipv4_settings));
    netmask_entry_widget = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_address_netmask_entry"));
    netmask_entry_res = (char *) gtk_entry_get_text(GTK_ENTRY(netmask_entry_widget));
    netmask_bits = atoi(netmask_entry_res);

    if ( !netmask_bits || netmask_bits < 1 || netmask_bits > 31 )
        netmask_bits = 24;

    while ( netmask_bits ) {
        ip_addr_settings->ipv4_netmask.netmask |= 1 << i;
        netmask_bits--;
        i--;
    }

    for (i = 0; i < IPv4_WIDGETS_N; i++) {
        snprintf(addr_widget_name, WIDGET_NAME_LEN, "ipv4_address_entry_%u", i+1);
        snprintf(gateway_widget_name, WIDGET_NAME_LEN, "ipv4_gateway_entry_%u", i+1);
        snprintf(dns_widget_name, WIDGET_NAME_LEN, "ipv4_dns_server_entry_%u", i+1);

        addr_entry_widget = GTK_WIDGET(gtk_builder_get_object(builder, addr_widget_name));
        gateway_entry_widget = GTK_WIDGET(gtk_builder_get_object(builder, gateway_widget_name));
        dns_entry_widget = GTK_WIDGET(gtk_builder_get_object(builder, dns_widget_name));

        addr_entry_res = (char *) gtk_entry_get_text(GTK_ENTRY(addr_entry_widget));
        gateway_entry_res = (char *) gtk_entry_get_text(GTK_ENTRY(gateway_entry_widget));
        dns_entry_res = (char *) gtk_entry_get_text(GTK_ENTRY(dns_entry_widget));

        if ( *addr_entry_res == '\0' )
            addr_set = false;

        if ( *gateway_entry_res == '\0' )
            gateway_set = false;

        if ( *dns_entry_res == '\0' )
            dns_set = false;

        addr_i = atoi(addr_entry_res);
        gateway_i = atoi(gateway_entry_res);
        dns_i = atoi(dns_entry_res);

        if ( i == 0 ) {
            if ( addr_i < 1 )
                addr_set = false;

            if ( gateway_i < 1 )
                gateway_set = false;

            if ( dns_i < 1 )
                gateway_set = false;
        }

        if ( addr_i >= 0 && addr_i <= 255 ) {
            //ipv4_addr[i] = addr_i;
            ip_addr_settings->ipv4_address.octets[i] = addr_i;
        }
        else
            addr_set = false;

        if ( gateway_i >= 0 && gateway_i <= 255 )
            ip_gateway[i] = gateway_i;
        else
            gateway_set = false;

        if ( dns_i >= 0 && dns_i <= 255 )
            ip_dns[i] = dns_i;
        else
            dns_set = false;

        //printf("%d %d %d\n", addr_i, gateway_i, dns_i);
        //printf("abc %s\n", gtk_entry_get_text(GTK_ENTRY(gateway_entry_widget)));
    }

    if ( !addr_set )
        ip_addr_settings = NULL;

    if ( !gateway_set )
        ip_gateway = NULL;

    if ( !dns_set )
        ip_dns = NULL;

    return 0;
}

static int apply_all_settings(GtkWidget *widget, GtkBuilder *builder)
{
    char ipv4_gateway[4], ipv4_dns[4];
    struct ipv4_settings ipv4_addr_info;
    gchar error_buf[ERROR_BUFFER_LEN];
    bool ipv4_success = false, ipv6_success = false;
    int ret;

    if ( !builder )
        return -1;

    if ( (ret = get_ipv4_settings_from_widgets(builder,
                           &ipv4_addr_info,
                           ipv4_gateway,
                           ipv4_dns
                           )) < 0 ) {
        snprintf(error_buf, ERROR_BUFFER_LEN,
                 "Cannot get IPv4 settings. Code: %d",
                 ret);
        show_alert("Error IPv4",
                   error_buf,
                   "OK");
        return -2;
    }

    if ( (ret = set_iw_ipv4_addr(selected_iw, ipv4_addr_info)) != 0 ) {
        snprintf(error_buf, ERROR_BUFFER_LEN,
                 "Cannot set IPv4 settings. Code: %d\n%s",
                 ret, strerror(ret));
        show_alert("Error IPv4",
                   error_buf,
                   "OK");
        return -3;
    } else
        show_alert("Success IPv4",
                   "IPv4 settings was applied successfully",
                   "OK");

    // Add set ipv6 settings
    return 0;
}

void show_net_settings_window(GtkMenuItem *settings)
{
    GtkBuilder *builder = gtk_builder_new_from_file("gui.ui");
    GtkWidget *window;

    if ( !settings || !builder )
        return;

    gtk_builder_connect_signals(builder, NULL);

    if ( !selected_iw ) {
        show_alert("The interface is not selected", "Please, select the interface", "OK");
        return;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "net_settings_window"));
    GtkWidget *static_ip_radio = GTK_WIDGET(gtk_builder_get_object(builder, "static_ip_settings_radio"));
    GtkWidget *dhcp_radio = GTK_WIDGET(gtk_builder_get_object(builder, "dhcp_radio"));
    GtkWidget *ipv4_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_enable"));
    GtkWidget *ipv6_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_enable"));
    GtkWidget *save_button = GTK_WIDGET(gtk_builder_get_object(builder, "apply_net_settings_button"));
    set_ipv4_settings_to_entry(builder);

    g_signal_connect(save_button, "clicked", G_CALLBACK(apply_all_settings), builder);
    g_signal_connect(static_ip_radio, "toggled", G_CALLBACK(enable_static_net_settings_sensitive), builder);
    g_signal_connect(dhcp_radio, "toggled", G_CALLBACK(disable_static_net_settings_sensitive), builder);
    g_signal_connect(ipv4_enable, "toggled", G_CALLBACK(enable_ipv4_settings), builder);
    g_signal_connect(ipv6_enable, "toggled", G_CALLBACK(enable_ipv6_settings), builder);

    gtk_window_set_title(GTK_WINDOW(window), "Network settings");
    gtk_widget_show_all(GTK_WIDGET(window));
}
