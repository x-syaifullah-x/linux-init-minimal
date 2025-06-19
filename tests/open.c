#include <unistd.h>

#include "../init.h"

void _start(void) {
    long fd_1, fd_2, ret;

    fd_1 = sys_c_open("/dev/null", O_RDONLY, NULL);
    if (fd_1 < 0) {
        sys_c_write(STDERR_FILENO, "Invalid open FD_1");
        sys_c_exit(fd_1);
    }

    fd_2 = sys_c_open("/dev/null", O_RDONLY, NULL);
    if (fd_2 < 0) {
        sys_c_write(STDERR_FILENO, "Invalid open FD_2");
        sys_c_exit(fd_2);
    }

    ret = sys_c_close(fd_1);
    if (ret < 0) {
        sys_c_write(STDERR_FILENO, "Invalid close FD_1");
        sys_c_exit(ret);
    }

    ret = sys_c_close(fd_2);
    if (ret < 0) {
        sys_c_write(STDERR_FILENO, "Invalid close FD_2");
        sys_c_exit(ret);
    }

    sys_c_exit(0);
}