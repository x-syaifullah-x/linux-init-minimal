#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#include "../init.h"

static inline long partuuid_to_devpath(const char *partuuid, char *out) {
    char link[256];
    char path[256];
    char *p;

    /* build "/dev/disk/by-partuuid/<UUID>" */
    p = path;
    const char *base = "/dev/disk/by-partuuid/";
    while (*base) *p++ = *base++;
    while (*partuuid) *p++ = *partuuid++;
    *p = 0;

    /* read symlink -> "../../sdb1" */
    long ret = sys_c_readlink(path, link, sizeof(link) - 1);
    if (ret <= 0)
        return ret;

    link[ret] = 0;

    /* resolve "../../sdb1" -> "/dev/sdb1" */
    p = out;
    *p++ = '/';
    *p++ = 'd';
    *p++ = 'e';
    *p++ = 'v';
    *p++ = '/';

    const char *q = link + 6; /* skip "../../" */
    while (*q) *p++ = *q++;
    *p = 0;

    return p - out;
}

void _start() {
    char buf[16];
    long len = partuuid_to_devpath("62d365b4-152f-451c-997c-bfff13ef9ec8", buf);

    struct stat st;

    // long fd = sys_c_open(buf, O_RDONLY, NULL);
    // if (fd < 0) {
    //     if (fd == -13) {
    //         sys_c_write(STDOUT_FILENO, "Permission dinied...");
    //     }
    //     sys_c_exit(1);
    // }
    // sys_c_fstat(fd, &st);
    // sys_c_close(fd);

    sys_c_stat(buf, &st);

    long major = gnu_dev_major(st.st_rdev);
    long minor = gnu_dev_minor(st.st_rdev);
    sys_c_write_num(STDOUT_FILENO, major);
    sys_c_write(STDOUT_FILENO, ":");
    sys_c_write_num(STDOUT_FILENO, minor);

    sys_c_write(STDOUT_FILENO, "\t");

    sys_c_write_buf(STDOUT_FILENO, buf, len);
    sys_c_write(STDOUT_FILENO, "\n");

    sys_c_exit(0);
}