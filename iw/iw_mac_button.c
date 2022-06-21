#include <gtk/gtk.h>
#include "interfaces.h"
#include "iw_mac_label.h"
#include "iw_mac_button.h"

extern char *selected_iw;

void iw_mac_random_button_show(GtkWidget *widget, GtkBuilder *builder)
{
    if ( !builder )
        return;

    GtkWidget *button;
    button = GTK_WIDGET(gtk_builder_get_object(builder, "random_mac_addr_button"));
    g_signal_connect(button, "clicked", G_CALLBACK(iw_mac_addr_randomize), builder);
    g_signal_connect(button, "clicked", G_CALLBACK(iw_mac_label_show), builder);
    gtk_widget_show(button);
}

void iw_mac_addr_randomize(GtkWidget *widget, GtkBuilder *builder)
{
    GtkWidget *mac_label;
    char mac_addr_str[MAC_ADDR_STR_LEN];

    if ( change_iw_mac_addr(selected_iw) != 0 )
        return;

    if ( get_iw_mac_addr(mac_addr_str) != 0 )
        return;

    mac_label = GTK_WIDGET(gtk_builder_get_object(builder, "mac_addr_label"));
    gtk_label_set_text(GTK_LABEL(mac_label), (const char *) mac_addr_str);
    gtk_widget_show(mac_label);
}
