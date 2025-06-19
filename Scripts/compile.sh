#/bin/sh

__C_FILE="$1"
__OUTPUT="${__C_FILE%.c}"
gcc -static -nostdlib -march=native -mtune=native -Ofast -fno-asynchronous-unwind-tables -fno-ident -fno-pie -ffreestanding -ffunction-sections -fdata-sections -fno-stack-protector -flto -s -o "Build/$__OUTPUT" "$__C_FILE" -Wl,--build-id=none,--gc-sections
