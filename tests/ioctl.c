#include <sys/ioctl.h>
#include <unistd.h>

#include "../init.h"

void _start(void) {
    long ret, fd, c;

    fd = sys_c_open("/dev/tty1", O_RDWR, NULL);
    struct winsize ws;

    ret = sys_c_ioctl(fd, TIOCGWINSZ, &ws);

    c = sys_c_close(fd);
    if (c < 0) {
        sys_c_write(STDERR_FILENO, "INVALID close fd");
        sys_c_exit(c);
    }

    if (ret < 0)
        sys_c_exit(ret);

    sys_c_write(STDOUT_FILENO, "row:\t");
    sys_c_write_num(STDOUT_FILENO, ws.ws_row);

    sys_c_write(STDOUT_FILENO, "\n");

    sys_c_write(STDOUT_FILENO, "col:\t");
    sys_c_write_num(STDOUT_FILENO, ws.ws_col);

    sys_c_exit(0);
}