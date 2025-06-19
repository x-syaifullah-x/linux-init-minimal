CURRENT_DIR="/$(realpath --relative-to=/ $(dirname $0) || exit $?)" 
OUT_DIR="$CURRENT_DIR/out"
mkdir -pv $OUT_DIR || exit $?

FILE_C=$1
[ -z "$FILE_C" ] && exit 1
OUT_FILE="$OUT_DIR/$(basename $FILE_C .c)"

gcc \
  -static \
  -nostdlib \
  -march=native \
  -mtune=native \
  -Ofast \
  -fno-asynchronous-unwind-tables \
  -fno-ident \
  -fno-pie \
  -ffreestanding \
  -ffunction-sections \
  -fdata-sections \
  -fno-stack-protector \
  -flto \
  -o "$OUT_FILE" "$FILE_C" \
  -Wl,--build-id=none,--gc-sections || exit $?
  
  $OUT_FILE