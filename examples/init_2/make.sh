#!/bin/sh

if [ "$#" -eq 0 ]; then
  echo "Usage: $(basename $0) --rootfs-dir <dir> --out </dev/sdxX | x.xxx>"
  exit 1
fi

while [ "$#" -gt 0 ]; do
  case "$1" in
    --rootfs-dir)
  		ROOTFS_DIR="$2"
      [ -z "$ROOTFS_DIR" ] && echo "Usage: $(basename $0) --rootfs-dir <dir>" && exit 1
      shift 2
      ;;

    --rootfs-dir=*)
        ROOTFS_DIR="${1#*=}"
        [ -z "$ROOTFS_DIR" ] && echo "Usage: $(basename $0) --rootfs-dir=<dir>" && exit 1
        shift
        ;;

  	--out)
  		OUT="$2"
      [ -z "$OUT" ] && echo "Usage: $(basename $0) --out <x.out | /dev/sdxX>" && exit 1
  		shift 2
  		;;
    
    --out=*)
        OUT="${1#*=}"
        [ -z "$OUT" ] && echo "Usage: $(basename $0) --out=<x.out | /dev/sdxX>" && exit 1
        shift
        ;;

    #--update-repo)
    #  _UPDATE_REPO=1
    #  shift
    #  ;;

    *)
      echo "Usage: $(basename $0) --rootfs-dir <dir> --out <x.out | /dev/sdxX>"
      exit 127
      ;;
  esac
done

[ -z "$ROOTFS_DIR" ] && echo "Usage: $(basename $0) --rootfs-dir <dir>" && exit 1
[ -z "$OUT" ] && echo "Usage: $(basename $0) --rootfs-dir $ROOTFS_DIR --out=<x.out | /dev/sdxX>" && exit 1


WORKING_DIR="/$(realpath --relative-to=/ $(dirname $0))"
OUT_DIR="$WORKING_DIR/out"

mkdir -pv $OUT_DIR

gcc \
  -static \
  -nostdlib \
  -march=native \
  -mtune=native \
  -Ofast \
  -fno-asynchronous-unwind-tables \
  -fno-ident \
  -fno-pie -ffreestanding \
  -ffunction-sections \
  -fdata-sections \
  -fno-stack-protector \
  -flto \
  -s \
  -o $OUT_DIR/init $WORKING_DIR/init.c \
  -Wl,--build-id=none,--gc-sections || exit $?

cp -rfv "$OUT_DIR/init" "$ROOTFS_DIR/sbin/x-init" || exit $?

mkfs.erofs -L ROOTFS --force-uid=0 --force-gid=0 "${OUT}" "${ROOTFS_DIR}" || exit $?

rm -rfv "${ROOTFS_DIR}/usr/sbin/x-init"
