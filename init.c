#include "init.h"

#include <unistd.h>

void _start() {
    sys_c_write_buf(STDOUT_FILENO, "test", sizeof("test"));
    sys_c_write(STDOUT_FILENO, "TEST");

    sys_c_exit(0);
}