#include <unistd.h>

#include "../init.h"

void _start() {
    long ret;
    char word[5] = "test\n";
    
    ret = sys_c_write_buf(STDOUT_FILENO, word, ARRAY_SIZE(word));
    if (ret != ARRAY_SIZE(word)) {
        sys_c_write(STDERR_FILENO, "Invalid.");
        sys_c_exit(ret);
    }

    sys_c_exit(0);
}