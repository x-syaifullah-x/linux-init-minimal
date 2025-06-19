#include <unistd.h>

#include "../init.h"

void _start() {
    long pid;

    pid = sys_c_fork();
    if (pid == 0) {
        static struct timespec ts = {.tv_sec = 3, .tv_nsec = 0};
        sys_c_sleep(&ts);
        sys_c_exit(1);
    }

    pid = sys_c_fork();
    if (pid == 0) {
        static struct timespec ts = {.tv_sec = 2, .tv_nsec = 0};
        sys_c_sleep(&ts);
        sys_c_exit(2);
    }

    pid = sys_c_fork();
    if (pid == 0) {
        static struct timespec ts = {.tv_sec = 1, .tv_nsec = 0};
        sys_c_sleep(&ts);
        sys_c_exit(3);
    }
    
    // long state;
    // long x = sys_c_waitpid(pid, &state, 0);
    // sys_c_write_num(STDOUT_FILENO, state /* state = 1 << 8 = 256 */);

    long status;
    long c_pid;
    while (1) {
        c_pid = sys_c_waitpid(-1, &status, 0);

        if (c_pid == -10) break;

        sys_c_write(STDOUT_FILENO, "[wait] child ");
        sys_c_write_num(STDOUT_FILENO, c_pid);
        sys_c_write(STDOUT_FILENO, " exited with code ");
        sys_c_write_num(STDOUT_FILENO, (status >> 8));
        sys_c_write(STDOUT_FILENO, "\n");
    }

    sys_c_write(STDOUT_FILENO, "Finish ...\n");

    sys_c_exit(0);
}