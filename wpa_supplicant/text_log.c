#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <sys/wait.h>
#include "files_job.h"
#include "text_log.h"
#include "wpa_subprocess.h"

static pthread_mutex_t wpa_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t wpa_read_tid, wpa_waitpid_tid;
pid_t wpa_supplicant_pid;

extern files_paths *files;
extern char *selected_iw;

static void *wait_pid_thread(void *arg)
{
    waitpid(wpa_supplicant_pid, NULL, 0);
    pthread_cancel(wpa_read_tid);
    pthread_mutex_unlock(&wpa_thread_mutex);
    return NULL;
}

static void *read_wpa_fd_out_thread(void *arg)
{
    GtkWidget *text = (GtkWidget *)arg;
    GtkTextBuffer *text_buffer;
    GtkTextView *text_view;
    int wpa_out_fd, n_read_wpa_pid;
    char buf[BUFSIZ+1];

    if ( !selected_iw )
        return NULL;

    if ( !files || !files->wpa_conf_file_path )
        return NULL;

    //printf("Selected iw: %s\n", selected_iw);
    //printf("%s\n", files->wpa_conf_file_path);
    char *args[] = {"wpa_supplicant", "-i", selected_iw, "-c", files->wpa_conf_file_path, NULL};

    if ( pthread_mutex_trylock(&wpa_thread_mutex) != 0 )
        return NULL;

    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    //text_view = gtk_text_view_new_with_buffer(text_buffer);
    wpa_supplicant_pid = wpa_supplicant_start(args, &wpa_out_fd);

    if ( wpa_supplicant_pid < 0 )
        return NULL;

    if ( pthread_create(&wpa_waitpid_tid, NULL, wait_pid_thread, NULL) != 0 ) {
        kill(wpa_supplicant_pid, SIGKILL);
        return NULL;
    }

    while ( (n_read_wpa_pid = read(wpa_out_fd, buf, BUFSIZ)) ) {
        buf[n_read_wpa_pid] = '\0';
        gtk_text_buffer_insert_at_cursor(text_buffer, (const gchar *)buf, (gint) -1);

        if ( files->log_wpa_supp_fd > 0 )
            write(files->log_wpa_supp_fd, buf, n_read_wpa_pid);

        //printf("%s", buf);
    }

    close(wpa_out_fd);
    return NULL;
}

void wpa_log_widget_enable(GtkWidget *widget, GtkWidget *text)
{
    pthread_create(&wpa_read_tid, NULL, read_wpa_fd_out_thread, (void *)text);
}

void wpa_log_widget_disable(GtkWidget *widget)
{
    if ( wpa_supplicant_pid )
        wpa_supplicant_stop(wpa_supplicant_pid);
}

void wpa_log_clear(GtkWidget *widget, GtkWidget *text_widget)
{
    GtkTextBuffer *text_buffer;
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_widget));
    gtk_text_buffer_set_text(text_buffer, (const gchar *) "\0", (gint) -1);
}
