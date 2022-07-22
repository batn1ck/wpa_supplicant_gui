#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wpa_subprocess.h"

pid_t wpa_supplicant_start(char **wpa_supplicant_argv, int *out_fd)
{
    pid_t wpa_supplicant_pid;
    int wpa_supplicant_fds[2];

    if ( !wpa_supplicant_argv )
        return -1;

    if ( pipe(wpa_supplicant_fds) < 0 )
        return -2;

    *out_fd = wpa_supplicant_fds[0];
    
    switch ( wpa_supplicant_pid = fork() ) {
     case 0:
         close(STDIN_FILENO);
         close(STDOUT_FILENO);
         close(STDERR_FILENO);
         close(wpa_supplicant_fds[0]);

         if ( dup2(wpa_supplicant_fds[1], STDOUT_FILENO) < 0 )
             exit(EXIT_FAILURE);

         if ( dup2(wpa_supplicant_fds[1], STDERR_FILENO) < 0 )
             exit(EXIT_FAILURE);
        
         execv("/usr/bin/wpa_supplicant", wpa_supplicant_argv);
    }

    return wpa_supplicant_pid;
}
