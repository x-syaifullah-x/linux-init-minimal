
#include "init.h"

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
    char buf[256];
    long len = partuuid_to_devpath("247d029e-e3ac-4e71-9e3f-408e0c9b6db5", buf);
    sys_c_write_buf(1, buf, len);

    sys_c_exit(0);
}