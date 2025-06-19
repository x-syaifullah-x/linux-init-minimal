#!/bin/sh

WORKING_DIR="/$(realpath --relative-to=/ $(dirname $0))"

OUT_DIR="$WORKING_DIR/out"
mkdir -pv $OUT_DIR

gcc -static -nostdlib -march=native -mtune=native -Ofast -fno-asynchronous-unwind-tables -fno-ident -fno-pie -ffreestanding -ffunction-sections -fdata-sections -fno-stack-protector -flto -s -o $OUT_DIR/init $WORKING_DIR/init.c -Wl,--build-id=none,--gc-sections

x="$OUT_DIR/initrd"
mkdir -pv ${x}/dev
mkdir -pv ${x}/root
mkdir -pv ${x}/run
mkdir -pv ${x}/sys
mknod -m 600 $x/dev/console c 5 1

cp -rfv $OUT_DIR/init $x

(
  cd $x
  find . | cpio -o -H newc > "$OUT_DIR/init.cpio"
)

rm -rfv $x