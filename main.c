#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include "wpa_supplicant/files_job.h"
#include "wpa_supplicant/text_log.h"
#include "iw/data_structure.h"
#include "iw/iw_combobox.h"
#include "iw/iw_mac_label.h"
#include "iw/iw_mac_button.h"

extern char *selected_iw;

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window, *combo;
    GtkWidget *log_wpa_supp, *wpa_button_start, *wpa_button_stop, *wpa_button_log_clear;

    builder = gtk_builder_new_from_file("gui.ui");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    combo = GTK_WIDGET(gtk_builder_get_object(builder, "interfaces_combo"));
    log_wpa_supp = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_text_log"));
    wpa_button_start = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_start"));
    wpa_button_stop = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_stop"));
    wpa_button_log_clear = GTK_WIDGET(gtk_builder_get_object(builder, "wpa_supplicant_log_clear"));

    combo_iw_show_interfaces(combo);

    gtk_window_set_title(GTK_WINDOW(window), "Wpa_supplicant");
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(combo, "changed", G_CALLBACK(iw_mac_label_show), builder);
    g_signal_connect(combo, "changed", G_CALLBACK(iw_mac_random_button_show), builder);
    g_signal_connect(wpa_button_start, "clicked", G_CALLBACK(wpa_log_widget_enable), log_wpa_supp);
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

  GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  files_paths_free();

  if ( selected_iw )
    free(selected_iw);

  return status;
}
