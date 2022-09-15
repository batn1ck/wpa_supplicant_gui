#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <gtk/gtk.h>
#include "files_job.h"

files_paths *files = NULL;

void wpa_conf_choose(GtkFileChooserButton *file_object)
{
    if ( file_object == NULL || files == NULL )
        return;

    files->wpa_conf_file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_object));
    //printf("Filename = %s\n", files->wpa_conf_file_path);
}

void log_wpa_supp_choose(GtkFileChooserButton *file_object)
{
    if ( file_object == NULL || files == NULL )
        return;

    char *log_wpa_supp_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_object));
    //printf("wpa supp log = %s\n", log_wpa_supp_path);

    if ( log_wpa_supp_path ) {
        files->log_wpa_supp_fd = open(log_wpa_supp_path, O_WRONLY | O_APPEND);

        if ( files->log_wpa_supp_fd < 0 )
            return;
    }
}

void log_ap_scan_choose(GtkFileChooserButton *file_object)
{
    if ( file_object == NULL || files == NULL )
        return;

    char *log_scan_ap_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_object));
    printf("ap log = %s\n", log_scan_ap_path);

}

int files_paths_init(void)
{
    files = (files_paths *) calloc(1, sizeof(files_paths));
    
    if ( files == NULL ) {
        fprintf(stderr, "Error in alloc memory for files_paths: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

void files_paths_free(void)
{
    if ( files->log_wpa_supp_fd > 0 )
        close(files->log_wpa_supp_fd);

    if ( files->log_scan_ap_fd > 0 )
        close(files->log_wpa_supp_fd);

    free(files);
}

files_paths *get_files_paths(void)
{
    return files;
}
