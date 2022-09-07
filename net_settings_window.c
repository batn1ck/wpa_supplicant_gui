#include <gtk/gtk.h>
#include "net_settings_window.h"

static void set_ipv4_settings_sensitive(GtkBuilder *builder, gboolean status)
{
    if ( !builder )
        return;

    GtkWidget *ipv4_addr_widget, *ipv4_gw_widget, *ipv4_dns_widget, *ipv4_netmask_widget;
    int i;
    char addr_widget_name[WIDGET_NAME_LEN], gw_widget_name[WIDGET_NAME_LEN], dns_widget_name[WIDGET_NAME_LEN];

    for (i = 1; i < IPv4_WIDGETS_N+1; i++) {
        snprintf(addr_widget_name, WIDGET_NAME_LEN, "ipv4_address_entry_%d", i);
        snprintf(gw_widget_name, WIDGET_NAME_LEN, "ipv4_gateway_entry_%d", i);
        snprintf(dns_widget_name, WIDGET_NAME_LEN, "ipv4_dns_server_entry_%d", i);

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

void show_net_settings_window(GtkMenuItem *settings)
{
    GtkBuilder *builder = gtk_builder_new_from_file("gui.ui");

    if ( !settings || !builder )
        return;

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "net_settings_window"));
    GtkWidget *static_ip_radio = GTK_WIDGET(gtk_builder_get_object(builder, "static_ip_settings_radio"));
    GtkWidget *dhcp_radio = GTK_WIDGET(gtk_builder_get_object(builder, "dhcp_radio"));
    GtkWidget *ipv4_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_enable"));
    GtkWidget *ipv6_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv6_enable"));

    g_signal_connect(static_ip_radio, "toggled", G_CALLBACK(enable_static_net_settings_sensitive), builder);
    g_signal_connect(dhcp_radio, "toggled", G_CALLBACK(disable_static_net_settings_sensitive), builder);
    g_signal_connect(ipv4_enable, "toggled", G_CALLBACK(enable_ipv4_settings), builder);
    g_signal_connect(ipv6_enable, "toggled", G_CALLBACK(enable_ipv6_settings), builder);

    gtk_window_set_title(GTK_WINDOW(window), "Network settings");
    gtk_widget_show_all(GTK_WIDGET(window));
}
