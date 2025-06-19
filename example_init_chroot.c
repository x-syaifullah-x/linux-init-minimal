#define _GNU_SOURCE

#include <linux/mount.h>

#include "init.h"

#define MD_MINOR 0
#define MD_NAME "md" STR(MD_MINOR)

#define FS "/dev/" MD_NAME
#define FS_TYPE "ext4"
#define FS_ROOT "/root"

#define O_BASE "/run"
#define O_UPPER O_BASE "/o_upper"
#define O_WORK O_BASE "/o_work"

//	INITRAMFS || INITRD:
//		dev	| init | proc | root | sys

void _start() {
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
    long fd = sys_c_open("/sys/module/md_mod/parameters/new_array", O_WRONLY, NULL);
    sys_c_write(fd, MD_NAME);
    sys_c_close(fd);

    md_disk_t disks = {{8, 1}, {8, 17}};
    md_a(sys_c_open(FS, O_RDONLY, NULL), MD_VERSION_1_2, disks, ARRAY_SIZE(disks));
		if (__builtin_expect(sys_c_mount(FS, FS_ROOT, FS_TYPE, MS_RDONLY | MS_NOATIME | MS_SILENT, NULL) < 0, 0))
			sys_c_exit(1);
    sys_c_chdir(FS_ROOT);
    sys_c_mount("/dev", "dev", NULL, MS_MOVE | MS_SILENT, NULL);
    sys_c_mount("/sys", "sys", NULL, MS_MOVE | MS_SILENT, NULL);
    sys_c_mount(FS_ROOT, "/", NULL, MS_MOVE | MS_SILENT, NULL);
    sys_c_chroot(".");

    sys_c_mount("tmpfs", "/run", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, "size=100%,mode=0755");
    if (sys_c_fork() == 0) {
        sys_c_mkdir(O_UPPER, 0755);
        sys_c_mkdir(O_WORK, 0755);

        sys_c_mkdir(O_UPPER "/boot", 0755);
        sys_c_mkdir(O_WORK "/boot", 0755);
        sys_c_mount("overlay", "/boot", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/boot,upperdir=" O_UPPER "/boot,workdir=" O_WORK "/boot");

        sys_c_mkdir(O_UPPER "/etc", 0755);
        sys_c_mkdir(O_WORK "/etc", 0755);
        sys_c_mount("overlay", "/etc", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/etc,upperdir=" O_UPPER "/etc,workdir=" O_WORK "/etc");

        sys_c_mkdir(O_UPPER "/home", 0755);
        sys_c_mkdir(O_WORK "/home", 0755);
        sys_c_mount("overlay", "/home", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/home,upperdir=" O_UPPER "/home,workdir=" O_WORK "/home");

        sys_c_mkdir(O_UPPER "/opt", 0755);
        sys_c_mkdir(O_WORK "/opt", 0755);
        sys_c_mount("overlay", "/opt", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/opt,upperdir=" O_UPPER "/opt,workdir=" O_WORK "/opt");

        sys_c_mkdir(O_UPPER "/root", 0700);
        sys_c_mkdir(O_WORK "/root", 0700);
        sys_c_mount("overlay", "/root", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/root,upperdir=" O_UPPER "/root,workdir=" O_WORK "/root");

        sys_c_mkdir(O_UPPER "/srv", 0755);
        sys_c_mkdir(O_WORK "/srv", 0755);
        sys_c_mount("overlay", "/srv", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/srv,upperdir=" O_UPPER "/srv,workdir=" O_WORK "/srv");

        sys_c_mkdir(O_UPPER "/usr", 0755);
        sys_c_mkdir(O_WORK "/usr", 0755);
        sys_c_mount("overlay", "/usr", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/usr,upperdir=" O_UPPER "/usr,workdir=" O_WORK "/usr");

        sys_c_mkdir(O_UPPER "/var", 0755);
        sys_c_mkdir(O_WORK "/var", 0755);
        sys_c_mount("overlay", "/var", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/var,upperdir=" O_UPPER "/var,workdir=" O_WORK "/var");

        sys_c_exit(0);
    }

    sys_c_mount("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL);
    // sys_c_write(sys_c_open("/proc/self/fd/1" /*STD_OUT*/, O_WRONLY, NULL), "Test Write To STD_OUT\n");
    // sys_c_write(sys_c_open("/proc/self/fd/2" /*STD_ERR*/, O_WRONLY, NULL), "Test Write To STD_ERR\n");

    sys_c_mount("ramfs", "/media", "ramfs", MS_NOATIME | MS_SILENT, "mode=0775");

    sys_c_mount("ramfs", "/mnt", "ramfs", MS_NOATIME | MS_SILENT, "mode=0775");

    sys_c_mount("ramfs", "/tmp", "ramfs", MS_NOSUID | MS_NODEV | MS_NOATIME | MS_SILENT, "mode=1777");

    char *argv[] = {"systemd", NULL};
    char *envp[] = {NULL};
    sys_c_execve("/lib/systemd/systemd", argv, envp);
}