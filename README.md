# KFS

KFS stands for Kernel from scratch. The goal is to build a functional kernel, without using any existing external code.
It's cut in 10 submodules.

**Current state:**

- [x] **KFS1:** Hello world kernel (multiboot + print screen)
- [x] **KFS2:** GDT and stack + basic shell
- [ ] **KFS3:** Memory and pagination
- [x] **KFS4:** Interrupts
- [ ] **KFS5:** Task switching and scheduling
- [ ] And 5 more...


## Usage

*To be able to build KFS, you will need a i686-elf cross toolchain. You will find information [here](https://wiki.osdev.org/GCC_Cross-Compiler) on how to build it for your distribution.*

### Clone the repo:
```
# with ssh
git clone git@github.com:lfalkau/kfs.git

# or with http
git clone https://github.com/lfalkau/kfs.git
```

### Run KFS with qemu:
```
cd kfs
make iso
make boot
```

You can alternatively run `make` if you just want to get the kernel binary image.
