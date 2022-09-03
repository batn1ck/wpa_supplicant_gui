#ifndef _FILES_JOB
#define _FILES_JOB

typedef struct {
    char *log_wpa_supp_path;
    char *log_scan_ap_path;
    char *wpa_conf_file_path;
} files_paths;

int files_paths_init(void);
void files_paths_free(void);
void wpa_conf_choose(GtkFileChooserButton *file_object);
void log_wpa_supp_choose(GtkFileChooserButton *file_object);
void log_ap_scan_choose(GtkFileChooserButton *file_object);

#endif
