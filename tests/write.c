#include <unistd.h>

#include "../init.h"

void _start() {
    long ret;
    const char *word = "test";

    ret = sys_c_write(STDOUT_FILENO, word);
    if (ret != 4)
        sys_c_exit(ret);

    sys_c_exit(0);
}