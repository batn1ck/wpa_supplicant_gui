#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <gtk/gtk.h>
#include "files_job.h"

files_paths *files = NULL;

void wpa_conf_choose(GtkFileChooserButton *file_object)
{
    if ( file_object == NULL || files == NULL )
        return;

    files->wpa_conf_file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_object));
    printf("Filename = %s\n", files->wpa_conf_file_path);
}

void log_wpa_supp_choose(GtkFileChooserButton *file_object)
{
    if ( file_object == NULL || files == NULL )
        return;

    files->log_wpa_supp_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_object));
    printf("wpa supp log = %s\n", files->log_wpa_supp_path);
}

void log_ap_scan_choose(GtkFileChooserButton *file_object)
{
    if ( file_object == NULL || files == NULL )
        return;

    files->log_scan_ap_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_object));
    printf("ap log = %s\n", files->log_scan_ap_path);
}

int files_paths_init(void)
{
    files = (files_paths *) malloc(sizeof(files_paths));
    
    if ( files == NULL ) {
        fprintf(stderr, "Error in alloc memory for files_paths: %s\n", strerror(errno));
        return -1;
    }

    memset(files, 0, sizeof(files_paths));
    return 0;
}

void files_paths_free(void)
{
    free(files);
}

files_paths *get_files_paths(void)
{
    return files;
}
