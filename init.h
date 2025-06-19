#ifndef _INIT_H
#define _INIT_H

#include <sys/fcntl.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <time.h>

#include "syscall/sys_c.h"

#undef NULL
#define NULL 0

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define STR_(x) #x
#define STR(x) STR_(x)

static inline void sys_c_exit(long __status) {
    sys_c_1(SYS_exit, __status);
}

static inline long sys_c_sleep(struct timespec *ts) {
    return sys_c_2(SYS_nanosleep, (long)ts, 0);
}

static inline long sys_c_open(const char *__path, long __oflag, unsigned long __mode) {
    return sys_c_4(SYS_openat, AT_FDCWD, (long)__path, __oflag, __mode);
}

static inline long sys_c_close(long __fd) {
    return sys_c_1(SYS_close, __fd);
}

static inline long sys_c_ioctl(long __fd, unsigned long __request, void *argp) {
    return sys_c_3(SYS_ioctl, __fd, __request, (long)argp);
}

static inline long sys_c_mount(const char *__special_file, const char *__dir, const char *__fstype, unsigned long __rwflag, const char *__data) {
    return sys_c_5(SYS_mount, (long)__special_file, (long)__dir, (long)__fstype, __rwflag, (long)__data);
}

static inline long sys_c_link(const char *__from, const char *__to) {
    return sys_c_3(SYS_symlinkat, (long)__from, AT_FDCWD, (long)__to);
}

static inline long sys_c_readlink(const char *__part_uuid, const char *__buffer, long __len) {
    return sys_c_3(SYS_readlink, (long)__part_uuid, (long)__buffer, (long)__len);
}

static inline long sys_c_mkdir(const char *__path, unsigned long __mode) {
    return sys_c_3(SYS_mkdirat, AT_FDCWD, (long)__path, __mode);
}

static inline long sys_c_chown(const char *__file, unsigned long __owner, unsigned long __group, long flag) {
    return sys_c_5(SYS_fchownat, AT_FDCWD, (long)__file, __owner, __group, flag);
}

static inline long sys_c_rm(const char *__path, long __flag) {
    return sys_c_3(SYS_unlinkat, AT_FDCWD, (long)__path, __flag);
}

static inline long sys_c_rm_dir(const char *__path) {
    return sys_c_rm(__path, AT_REMOVEDIR);
}

static inline long sys_c_execve(const char *__path, char *const __argv[], char *const __envp[]) {
    return sys_c_3(SYS_execve, (long)__path, (long)__argv, (long)__envp);
}

static inline long sys_c_chroot(const char *__path) {
    return sys_c_1(SYS_chroot, (long)__path);
}

static inline long sys_c_pivot_root(const char *__new_root, const char *__old_root) {
    return sys_c_2(SYS_pivot_root, (long)__new_root, (long)__old_root);
}

/**
 * sys_c_fork() - fork a new process
 *
 * Returns:
 *   0  in the child process
 *  >0  child PID in the parent process
 *  <0  negative error code (-errno) on failure
 */
static inline long sys_c_fork() {
    return sys_c_0(SYS_fork);
}

static inline long sys_c_chdir(const char *__path) {
    return sys_c_1(SYS_chdir, (long)__path);
}

/**
 * __fd     : File descriptor
 * __buf    : Data buffer
 * __n      : Byte count
 *
 * return   : Always __n
 */
static inline long sys_c_write_buf(long __fd, const void *__buf, size_t __n) {
    return sys_c_3(SYS_write, __fd, (long)(__buf), __n);
}

unsigned long strlen(const char *s) {
    const char *start = s;
    while (*s) ++s;
    return s - start;
}

static inline long sys_c_write(long __fd, const char *__s) {
    return sys_c_write_buf(__fd, __s, __builtin_strlen(__s) /* Fallback to strlen */);
}

static inline unsigned long sys_c_ltoa(long long v, char *b) {
    char *p = b;
    long long neg = v < 0;
    if (neg) v = -v;
    char *s = p;
    do {
        *p++ = '0' + (v % 10);
        v /= 10;
    } while (v);
    if (neg) *p++ = '-';
    for (char *e = p - 1; s < e; s++, e--) {
        char tmp = *s;
        *s = *e;
        *e = tmp;
    }
    return p - b;
}

static inline long sys_c_write_num(long __fd, long long __num) {
    char buf[21];
    return sys_c_3(SYS_write, __fd, (long)buf, sys_c_ltoa(__num, buf));
}

// static inline long sys_c_lseek(int __fd, long __offset, int __whence) {
//     return sys_c_3(SYS_lseek, __fd, __offset, __whence);
// }

// static inline long sys_c_init_module(void *__module_image, const unsigned long __len, const char *__param) {
//     return sys_c_3(SYS_init_module, (long)__module_image, __len, (long)__param);
// }

// static inline void *sys_c_mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd, __off_t __offset) {
//     return (void *)sys_c_6(SYS_mmap, (long)__addr, __len, __prot, __flags, __fd, __offset);
// }

// static inline long sys_c_munmap(void *__addr, size_t __len) {
//     return sys_c_2(SYS_munmap, (long)__addr, __len);
// }

// static inline long sys_c_read(int __fd, void *__buf, unsigned long __nbytes) {
//     return sys_c_3(SYS_read, __fd, (long)__buf, __nbytes);
// }

static inline long sys_c_fstat(long __fd, struct stat *__restrict __buf) {
    return sys_c_2(SYS_fstat, __fd, (long)__buf);
}

static inline long sys_c_stat(const char *__restrict __file, struct stat *__restrict __buf) {
    return sys_c_2(SYS_stat, (long)__file, (long)__buf);
}

static inline long sys_c_mknod(const char *__path, unsigned long __mode, unsigned long __dev) {
    return sys_c_4(SYS_mknodat, AT_FDCWD, (long)__path, __mode, __dev);
}

static inline long sys_c_umount(const char *__special_file, long __flags) {
    return sys_c_2(SYS_umount2, (long)__special_file, __flags);
}

static inline long sys_c_umount_r(const char *__special_file) {
    return sys_c_2(SYS_umount2, (long)__special_file, MNT_DETACH);
}

static inline long sys_c_waitpid_ru(long __pid, long *__stat_loc, long __options, struct rusage *ru) {
    return sys_c_4(SYS_wait4, __pid, (long)__stat_loc, __options, (long)ru);
}

static inline long sys_c_waitpid(long __pid, long *__stat_loc, long __options) {
    return sys_c_4(SYS_wait4, __pid, (long)__stat_loc, __options, 0);
}

#include <linux/ioctl.h>
#include <linux/major.h>
#include <linux/raid/md_u.h>

typedef const unsigned long md_version_t[2];

#define MD_VERSION_0_90 ((md_version_t){0, 90})
#define MD_VERSION_1_0 ((md_version_t){1, 0})
#define MD_VERSION_1_1 ((md_version_t){1, 1})
#define MD_VERSION_1_2 ((md_version_t){1, 2})

typedef const unsigned long md_disk_t[][2];

static inline mdu_array_info_t md_get_array_info(const char *__file) {
    long fd = sys_c_open(__file, O_RDONLY | __O_NOATIME, NULL);
    mdu_array_info_t info_array;
    sys_c_ioctl(fd, GET_ARRAY_INFO, &info_array);
    sys_c_close(fd);
    return info_array;
}

static inline long md_assemble(long fd, md_version_t __versions, md_disk_t __disks, const unsigned long __n_disk) {
    mdu_array_info_t info_array = {.major_version = __versions[0], .minor_version = __versions[1]};
    sys_c_ioctl(fd, SET_ARRAY_INFO, &info_array);
    mdu_disk_info_t info_disk;
    for (unsigned long i = 0; i < __n_disk; i++) {
        info_disk.major = __disks[i][0];
        info_disk.minor = __disks[i][1];
        sys_c_ioctl(fd, ADD_NEW_DISK, &info_disk);
    }
    return sys_c_ioctl(fd, RUN_ARRAY, NULL);
}
#endif  // _INIT_H