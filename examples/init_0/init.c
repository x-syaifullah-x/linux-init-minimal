#define _GNU_SOURCE

#include "../../init.h"

#include <linux/mount.h>

#define MD_MINOR 0
#define MD_NAME "md" STR(MD_MINOR)
#define MD_DEVICE "/dev/" MD_NAME

#define FS "/dev/sda1"
#define FS_TYPE "ext4"
#define FS_ROOT "/root"

#define O_BASE "/run"
#define O_LOWER O_BASE "/o_lower"
#define O_UPPER O_BASE "/o_upper"
#define O_WORK O_BASE "/o_work"

void _start() {
    long ret;

    ret = sys_c_mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);

    ret = sys_c_mount("udev", "/dev", "devtmpfs", MS_NOSUID | MS_NOATIME | MS_SILENT, "mode=0755");
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);
    if (sys_c_fork() == 0) {
        sys_c_mkdir("/dev/shm", 1777);
        sys_c_mount("tmpfs", "/dev/shm", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOATIME | MS_SILENT, "size=100%,huge=always");
        sys_c_exit(0);
    }

    ret = sys_c_mount("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);
    if (sys_c_fork() == 0) {
        sys_c_write(sys_c_open("/sys/kernel/mm/transparent_hugepage/defrag", O_WRONLY, NULL), "defer");
        sys_c_write(sys_c_open("/sys/kernel/mm/transparent_hugepage/shmem_enabled", O_WRONLY, NULL), "within_size");
        sys_c_exit(0);
    }

    ret = sys_c_mount("tmpfs", "/run", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, "size=100%,mode=0755");
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_mount(FS, FS_ROOT, FS_TYPE, MS_RDONLY | MS_NOATIME | MS_SILENT, NULL);
    if (unlikely(ret < 0))
        sys_c_exit(ret);
    sys_c_mkdir(O_UPPER, 0755);
    sys_c_mkdir(O_WORK, 0755);
    ret = sys_c_mount("overlay", FS_ROOT, "overlay", MS_NOATIME | MS_SILENT, "lowerdir=" FS_ROOT ",upperdir=" O_UPPER ",workdir=" O_WORK);
    if (unlikely(ret < 0))
        sys_c_exit(ret);
    // if (sys_c_fork() == 0) {
    //     ret = sys_c_umount(O_LOWER, 2 /* MNT_DETACH */);
    //     if (ret >= 0)
    //         sys_c_rm_dir(O_LOWER);
    //     sys_c_exit(0);
    // }

    ret = sys_c_chdir(FS_ROOT);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_mount("proc", "proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_mount("/dev", "dev", NULL, MS_MOVE | MS_SILENT, NULL);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_mount("/run", "run", NULL, MS_MOVE | MS_SILENT, NULL);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_mount("/sys", "sys", NULL, MS_MOVE | MS_SILENT, NULL);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_mount(FS_ROOT, "/", NULL, MS_MOVE | MS_SILENT, NULL);
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_chroot(".");
    // if (unlikely(ret < 0))
    //     sys_c_exit(ret);

    ret = sys_c_write(sys_c_open("/sys/module/md_mod/parameters/new_array", O_WRONLY, NULL), MD_NAME);
    md_disk_t disks = {{8, 17}, {8, 33}};
    ret = md_assemble(sys_c_open(MD_DEVICE, O_RDONLY, NULL), MD_VERSION_1_2, disks, ARRAY_SIZE(disks));

    ret = sys_c_mount(MD_DEVICE, "/home", FS_TYPE, MS_RDONLY | MS_NOATIME | MS_SILENT, NULL);
    sys_c_mkdir(O_UPPER "/home", 0755);
    sys_c_mkdir(O_WORK "/home", 0755);
    sys_c_mount("overlay", "/home", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/home,upperdir=" O_UPPER "/home,workdir=" O_WORK "/home");

    char *argv[] = {"systemd", NULL};
    char *envp[] = {NULL};
    sys_c_execve("/lib/systemd/systemd", argv, envp);
}
