#define _GNU_SOURCE

#include "../../init.h"

#include <linux/mount.h>

#define DEV_DIR "/dev"
#define DEV_SHM_DIR DEV_DIR "/shm"

#define SYSFS_DIR "/sys"

#define PROC_DIR "/proc"

#define RUNTIME_DIR "/run"

#define O_LOWER RUNTIME_DIR "/o_lower"
#define O_UPPER RUNTIME_DIR "/o_upper"
#define O_WORK RUNTIME_DIR "/o_work"

#define MD_MINOR 0
#define MD_NAME "md" STR(MD_MINOR)
#define MD_DEV DEV_DIR "/" MD_NAME
#define MD_TYPE "ext4"

#define DATA O_LOWER "/data"

void _start() {
    sys_c_mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);

    sys_c_mount("udev", DEV_DIR, "devtmpfs", MS_NOSUID | MS_NOATIME | MS_SILENT, "mode=0755");
    if (sys_c_fork() == 0) {
        sys_c_mkdir(DEV_SHM_DIR, 1777);
        sys_c_mount("tmpfs", DEV_SHM_DIR, "tmpfs", MS_NOSUID | MS_NODEV | MS_NOATIME | MS_SILENT, "size=100%,huge=always");
        sys_c_exit(0);
    }

    if (sys_c_mount("none", RUNTIME_DIR, "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, "size=100%,mode=0755") >= 0) {
        sys_c_mkdir(O_LOWER, 0755);
        sys_c_mkdir(O_UPPER, 0755);
        sys_c_mkdir(O_WORK, 0755);

        if (sys_c_fork() == 0) {
            if (sys_c_fork() == 0) {
                if (sys_c_mount("sysfs", SYSFS_DIR, "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL) >= 0) {
                    sys_c_write(sys_c_open(SYSFS_DIR "/kernel/mm/transparent_hugepage/defrag", O_WRONLY, NULL), "defer");
                    sys_c_write(sys_c_open(SYSFS_DIR "/kernel/mm/transparent_hugepage/shmem_enabled", O_WRONLY, NULL), "within_size");

                    sys_c_write(sys_c_open(SYSFS_DIR "/module/md_mod/parameters/new_array", O_WRONLY, NULL), MD_NAME);
                    md_disk_t disks = {{8 /* MAJOR */, 1 /* MINOR */}, {8 /* MAJOR */, 17 /* MINOR */}};
                    // md_disk_t disks = {{8 /* MAJOR */, 17 /* MINOR */}, {8 /* MAJOR */, 33 /* MINOR */}};
                    md_assemble(sys_c_open(MD_DEV, O_RDONLY, NULL), MD_VERSION_1_2, disks, ARRAY_SIZE(disks));

                    sys_c_mkdir(DATA, 0755);

                    sys_c_mount(MD_DEV, DATA, MD_TYPE, MS_RDONLY | MS_NOATIME | MS_SILENT, NULL);

                    sys_c_mkdir(O_UPPER "/home", 0755);
                    sys_c_mkdir(O_WORK "/home", 0755);
                    sys_c_mount("none", "/home", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=" DATA "/home,upperdir=" O_UPPER "/home,workdir=" O_WORK "/home");
                }
                sys_c_exit(0);
            }

            sys_c_mount("none", "/boot", "ramfs", MS_NOATIME | MS_SILENT, "mode=0755");

            sys_c_mkdir(O_UPPER "/etc", 0755);
            sys_c_mkdir(O_WORK "/etc", 0755);
            sys_c_mount("none", "/etc", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/etc,upperdir=" O_UPPER "/etc,workdir=" O_WORK "/etc");

            sys_c_mount("none", "/media", "ramfs", MS_NOATIME | MS_SILENT, "mode=0755");

            sys_c_mount("none", "/mnt", "ramfs", MS_NOATIME | MS_SILENT, "mode=0755");

            sys_c_mount("none", "/opt", "ramfs", MS_NOATIME | MS_SILENT, "mode=0755");

            sys_c_mount("none", "/root", "ramfs", MS_NOATIME | MS_SILENT, "mode=0700");

            sys_c_mount("none", "/srv", "ramfs", MS_NOATIME | MS_SILENT, "mode=0755");

            sys_c_mount("none", "/tmp", "tmpfs", MS_NOATIME | MS_NOSUID | MS_NODEV | MS_SILENT, "size=100%,mode=1777");

            sys_c_mkdir(O_UPPER "/usr", 0755);
            sys_c_mkdir(O_WORK "/usr", 0755);
            if (sys_c_mount("none", "/usr", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/usr,upperdir=" O_UPPER "/usr,workdir=" O_WORK "/usr") >= 0) {
                sys_c_rm("/usr/sbin/x-init", NULL);
            }

            sys_c_mkdir(O_UPPER "/var", 0755);
            sys_c_mkdir(O_WORK "/var", 0755);
            sys_c_mount("none", "/var", "overlay", MS_NOATIME | MS_SILENT, "lowerdir=/var,upperdir=" O_UPPER "/var,workdir=" O_WORK "/var");

            sys_c_exit(0);
        }
    }

    sys_c_mount("proc", PROC_DIR, "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME | MS_SILENT, NULL);

    char *argv[] = {"systemd", NULL};
    char *envp[] = {NULL};
    sys_c_execve("/usr/lib/systemd/systemd", argv, envp);
}
