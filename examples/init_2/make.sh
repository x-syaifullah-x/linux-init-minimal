#!/bin/sh

WORKING_DIR="/$(realpath --relative-to=/ $(dirname $0))"

OUT_DIR="$WORKING_DIR/out"
mkdir -pv $OUT_DIR

SYSROOT="/r"
ROOTFS_IMAGE=/rootfs.erofs
gcc -DROOTFS_IMAGE="\"$ROOTFS_IMAGE\"" -DSYSROOT="\"$SYSROOT\"" -static -nostdlib -march=native -mtune=native -Ofast -fno-asynchronous-unwind-tables -fno-ident -fno-pie -ffreestanding -ffunction-sections -fdata-sections -fno-stack-protector -flto -s -o $OUT_DIR/init $WORKING_DIR/init.c -Wl,--build-id=none,--gc-sections

x="$OUT_DIR/initrd"
mkdir -pv ${x}/dev
mkdir -pv ${x}/run
mkdir -pv ${x}/sys
mkdir -pv ${x}$SYSROOT
mknod -m 600 ${x}/dev/console c 5 1

ROOTFS_DIR=/media/xxx/sdc2/xxx/Rootfs/trixie-x86_64-qemu

# SQUASHFS
# mksquashfs $ROOTFS_DIR $x/rootfs.squashfs -b 1M -noappend -comp lz4 -no-compression -no-xattrs -no-exports || exit $?
# mksquashfs $ROOTFS_DIR $x/rootfs.squashfs -b 1M -noappend -comp lz4 -Xhc -no-xattrs -no-exports || exit $?
# mksquashfs $ROOTFS_DIR $x/rootfs.squashfs -b 1M -noappend -comp lzo -Xcompression-level 9 -no-xattrs -no-exports || exit $?

# EROFS
mkfs.erofs -L rootfs --force-uid=0 --force-gid=0 "${x}${ROOTFS_IMAGE}" $ROOTFS_DIR || exit $?
# mkfs.erofs $x/rootfs.erofs $ROOTFS_DIR -L rootfs -z lz4 --force-uid=0 --force-gid=0 || exit $?
# mkfs.erofs $x/rootfs.erofs $ROOTFS_DIR -L rootfs -z lz4hc,level=12 --force-uid=0 --force-gid=0 || exit $?

# ISO
# genisoimage -o $x/rootfs.iso -R $ROOTFS_DIR || exit $?

cp -rfv $OUT_DIR/init $x
cp -rfv /media/xxx/sdc2/xxx/Git/x-syaifullah-x/linux-kernel-lenovo-B490/Microcode/kernel $x || exit $?

(
  cd $x
  find . | cpio -o -H newc > "$OUT_DIR/init.cpio" || exit $?
)

rm -rfv $x
