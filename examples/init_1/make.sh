#!/bin/sh

WORKING_DIR="/$(realpath --relative-to=/ $(dirname $0))"

OUT_DIR="$WORKING_DIR/out"
mkdir -pv $OUT_DIR

gcc -static -nostdlib -march=native -mtune=native -Ofast -fno-asynchronous-unwind-tables -fno-ident -fno-pie -ffreestanding -ffunction-sections -fdata-sections -fno-stack-protector -flto -s -o $OUT_DIR/init $WORKING_DIR/init.c -Wl,--build-id=none,--gc-sections || exit 1

x="$OUT_DIR/initrd"
rm -rfv $x
cp -rfvp /media/xxx/sdc2/xxx/Rootfs/trixie-x86_64-qemu $x
cp -rfv $OUT_DIR/init $x

(
  cd $x
  find . | cpio -o -H newc > "$OUT_DIR/init.cpio"
)

rm -rfv $x