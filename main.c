#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "wpa_supplicant/files_job.h"
#include "wpa_supplicant/text_log.h"
#include "wpa_supplicant/wpa_subprocess.h"
#include "iw/data_structure.h"
#include "iw/iw_combobox.h"
#include "iw/iw_mac_label.h"
#include "iw/iw_mac_button.h"

extern char *selected_iw;
extern pid_t wpa_supplicant_pid;

void close_app(GtkWidget *widget)
{
    files_paths_free();

    if ( selected_iw )
        free(selected_iw);

    if ( wpa_supplicant_pid > 0 )
        wpa_supplicant_stop(wpa_supplicant_pid);
}

//void enable_static_net_settings_sensitive(GtkBuilder *builder)
//{
//    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, ""));
//}

void show_net_settings(GtkMenuItem *settings, GtkBuilder *builder)
{
    if ( !settings || !builder )
        return;

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "net_settings_window"));
    //GtkWidget *ipv4_enable = GTK_WIDGET(gtk_builder_get_object(builder, "ipv4_enable"));
    //gtk_widget_set_sensitive(ipv4_enable, FALSE);

    gtk_window_set_title(GTK_WINDOW(window), "Network settings");
    gtk_widget_show(GTK_WIDGET(window));
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window, *combo;
    GtkWidget *log_wpa_supp, *wpa_button_start, *wpa_button_stop, *wpa_button_log_clear;
    GtkWidget *net_settings_tab;

    builder = gtk_builder_new_from_file("gui.ui");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    net_settings_tab = GTK_WIDGET(gtk_builder_get_object(builder, "net_settings_tab"));
    combo = GTK_WIDGET(gtk_builder_get_object(builder, "interfaces_combo"));
    log_wpa_supp = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_text_log"));
    wpa_button_start = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_start"));
    wpa_button_stop = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_stop"));
    wpa_button_log_clear = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_log_clear"));

    combo_iw_show_interfaces(combo);

    gtk_window_set_title(GTK_WINDOW(window), "Wpa_supplicant");
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(close_app), builder);
    g_signal_connect(net_settings_tab, "activate", G_CALLBACK(show_net_settings), builder);
    g_signal_connect(combo, "changed", G_CALLBACK(iw_mac_label_show), builder);
    g_signal_connect(combo, "changed", G_CALLBACK(iw_mac_random_button_show), builder);
    g_signal_connect(wpa_button_start, "clicked", G_CALLBACK(wpa_log_widget_enable), log_wpa_supp);
    g_signal_connect(wpa_button_stop, "clicked", G_CALLBACK(wpa_log_widget_disable), NULL);
    g_signal_connect(wpa_button_log_clear, "clicked", G_CALLBACK(wpa_log_clear), log_wpa_supp);

    //choose = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_conf_select"));

    gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
    gtk_widget_show(window);
    //gtk_widget_show_all(GTK_WIDGET(window));
}

int
main (int   argc,
      char *argv[])
{
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif

  if ( files_paths_init() != 0 )
      exit(EXIT_FAILURE);

  GtkApplication *app = gtk_application_new ("org.gtk.exampleeee", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
