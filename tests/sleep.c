#include "../init.h"

void _start() {
    long ret;

    static struct timespec ts = {.tv_sec = 1, .tv_nsec = 0};
    ret = sys_c_sleep(&ts);

    if (ret < 0)
        sys_c_exit(ret);
    sys_c_exit(0);
}