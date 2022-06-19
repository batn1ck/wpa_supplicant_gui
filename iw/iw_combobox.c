#include <gtk/gtk.h>
#include <stdio.h>
#include "interfaces.h"
#include "iw_combobox.h"

void combo_iw_entry(GtkEntry *entry_if)
{
    printf("%s\n", gtk_entry_get_text(entry_if));
}

GtkWidget *combo_iw_show_interfaces(GtkWidget *combo)
{
    GtkListStore *list_store;
    list_store = gtk_list_store_new(1, G_TYPE_STRING);
    struct iw_info *iw_start, *iw_last;

    iw_start = get_iw_info();
    iw_last = iw_start;

    while ( iw_last != NULL ) {
        gtk_list_store_insert_with_values(list_store, NULL, -1,
                                          0, iw_last->iw_name,
                                          -1);
        iw_last = iw_last->iw_next;
    }

    clear_iw_list(iw_start);

    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(list_store));
    gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combo), 0);
    g_object_unref(list_store);

    return combo;
}
