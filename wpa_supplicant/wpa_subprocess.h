#ifndef _WPA_SUBPROC
#define _WPA_SUBPROC

pid_t wpa_supplicant_start(char **, int *);
int wpa_supplicant_stop(pid_t);

#endif
