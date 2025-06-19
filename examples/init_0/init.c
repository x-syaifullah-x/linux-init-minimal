#define _GNU_SOURCE

#include "../../init.h"

#include <linux/mount.h>

#define MD_MINOR 0
#define MD_NAME "md" STR(MD_MINOR)
#define MD_DEVICE "/dev/" MD_NAME

// #define FS "/dev/sda1"
#define FS "/dev/sdb4"
#define FS_TYPE "ext4"
#define FS_ROOT "/root"

#define O_BASE "/run"
#define O_LOWER O_BASE "/o_lower"
#define O_UPPER O_BASE "/o_upper"
#define O_WORK O_BASE "/o_work"

void _start() {
    sys_c_mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);

    sys_c_mount("udev", "/dev", "devtmpfs", MS_NOSUID | MS_NOATIME | MS_SILENT, "mode=0755");
    if (sys_c_fork() == 0) {
        sys_c_mkdir("/dev/shm", 1777);
        sys_c_mount("tmpfs", "/dev/shm", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOATIME | MS_SILENT, "size=100%,huge=always");
        sys_c_exit(0);
    }

    sys_c_mount("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL);
    if (sys_c_fork() == 0) {
        sys_c_write(sys_c_open("/sys/kernel/mm/transparent_hugepage/defrag", O_WRONLY, NULL), "defer");
        sys_c_write(sys_c_open("/sys/kernel/mm/transparent_hugepage/shmem_enabled", O_WRONLY, NULL), "within_size");
        sys_c_exit(0);
    }

    sys_c_mount("tmpfs", "/run", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, "size=100%,mode=0755");

    sys_c_mkdir(O_LOWER, 0755);
    sys_c_mount(FS, O_LOWER, FS_TYPE, MS_RDONLY | MS_NOATIME | MS_SILENT, NULL);
    sys_c_mkdir(O_UPPER, 0755);
    sys_c_mkdir(O_WORK, 0755);
    sys_c_mount("overlay", FS_ROOT, "overlay", MS_NOATIME | MS_SILENT, "lowerdir=" O_LOWER ",upperdir=" O_UPPER ",workdir=" O_WORK);

    sys_c_chdir(FS_ROOT);

    sys_c_mount("proc", "proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL);

    sys_c_mount("/dev", "dev", NULL, MS_MOVE | MS_SILENT, NULL);

    sys_c_mount("/run", "run", NULL, MS_MOVE | MS_SILENT, NULL);

    sys_c_mount("/sys", "sys", NULL, MS_MOVE | MS_SILENT, NULL);

    sys_c_mount(FS_ROOT, "/", NULL, MS_MOVE | MS_SILENT, NULL);

    sys_c_chroot(".");

    sys_c_write(sys_c_open("/sys/module/md_mod/parameters/new_array", O_WRONLY, NULL), MD_NAME);
    md_disk_t disks = {{8, 1}, {8, 17}};
    // md_disk_t disks = {{8, 17}, {8, 33}};
    md_assemble(sys_c_open(MD_DEVICE, O_RDONLY, NULL), MD_VERSION_1_2, disks, ARRAY_SIZE(disks));

    sys_c_mount(MD_DEVICE, O_LOWER "/mnt", FS_TYPE,  MS_RDONLY | MS_NOATIME | MS_SILENT, NULL);
    sys_c_mkdir(O_UPPER "/home", 0755);
    sys_c_mkdir(O_WORK "/home", 0755);
    sys_c_mount("overlay", "/home", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=" O_LOWER "/mnt/home,upperdir=" O_UPPER "/home,workdir=" O_WORK "/home");

    //if (sys_c_fork() == 0) {
    //	sys_c_umount_r(O_LOWER);
    //}

    char *argv[] = {"systemd", NULL};
    char *envp[] = {NULL};
    sys_c_execve("/lib/systemd/systemd", argv, envp);
}
