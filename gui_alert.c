#include <gtk/gtk.h>
#include "gui_alert.h"

void show_alert(gchar *title_text,
                       gchar *body_label_text,
                       gchar *button_text)
{
    GtkWidget *dialog;
    GtkWidget *button_ok, *label_dialog, *content_dialog_area;

    if ( !title_text || !body_label_text || !button_text )
        return;

    dialog = gtk_dialog_new();
    content_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_NONE);
    gtk_window_set_title(GTK_WINDOW(dialog), title_text);
    button_ok = gtk_button_new_with_label(button_text);

    gtk_widget_set_halign(button_ok, GTK_ALIGN_CENTER);
    label_dialog = gtk_label_new(body_label_text);
    g_signal_connect_swapped(button_ok, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);
    gtk_container_add(GTK_CONTAINER(content_dialog_area), label_dialog);
    gtk_container_add(GTK_CONTAINER(content_dialog_area), button_ok);
    gtk_widget_show_all(dialog);
}
