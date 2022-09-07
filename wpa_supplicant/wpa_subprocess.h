#ifndef WPA_SUBPROC_H
#define WPA_SUBPROC_H

pid_t wpa_supplicant_start(char **, int *);
int wpa_supplicant_stop(pid_t);

#endif
