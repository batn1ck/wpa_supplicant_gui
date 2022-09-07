#ifndef FILES_JOB_H
#define FILES_JOB_H

typedef struct {
    int log_wpa_supp_fd;
    int log_scan_ap_fd;
    char *wpa_conf_file_path;
} files_paths;

int files_paths_init(void);
void files_paths_free(void);
void wpa_conf_choose(GtkFileChooserButton *file_object);
void log_wpa_supp_choose(GtkFileChooserButton *file_object);
void log_ap_scan_choose(GtkFileChooserButton *file_object);

#endif
