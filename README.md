# INIT MINIMAL
#
### INSTALL TOOLS AND DEPENDENCIES
```sh
sudo apt install --no-install-suggests --no-install-recommends gcc libc-dev
```

### COMPILE INIT
```sh
gcc -static -nostdlib -march=native -mtune=native -Ofast -fno-asynchronous-unwind-tables -fno-ident -fno-pie -ffreestanding -ffunction-sections -fdata-sections -fno-stack-protector -flto -s -o out/init init.c -Wl,--build-id=none,--gc-sections
```

### COMPILE INIT CHROOT
```sh
gcc -static -nostdlib -march=native -mtune=native -Ofast -fno-asynchronous-unwind-tables -fno-ident -fno-pie -ffreestanding -ffunction-sections -fdata-sections -fno-stack-protector -flto -s -o Build/init_chroot example_init_chroot.c -Wl,--build-id=none,--gc-sections
```