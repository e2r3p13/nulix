
cross-target:= i686-elf
target:= i386

kernel-name:= kfs
kernel-version:= 0.0.1
kernel:= ${kernel-name}-${kernel-version}

#BUILD 
subdir+= kernel lib

archdir:= arch/${target}

builddir?= $(shell pwd)/build
isodir:= ${builddir}/iso

.INCLUDE_DIRS:= $(shell pwd)/include

objs= $(shell find ${builddir} -type f -name "*.o")
libs= $(shell find ${builddir} -type f -name "*.a")

LD:= ${cross-target}-ld
LDFLAGS+= -T ${archdir}/linker.ld
QEMU:= qemu-system-i386
QEMUFLAGS+= -serial stdio
GRUBMK:=grub2-mkrescue
GRUBMKFLAGS+=--compress=xz
#GRUBMOD:=--install-modules="normal multiboot2 part_gpt part_acorn part_apple\
#		part_bsd part_amiga part_dfly part_dvh part_plan part_sun part_sunpc"

.PHONY: all
all: build link

.PHONY: build
build: build-subdir

.PHONY: link
link: build
	@${LD} ${LDFLAGS} -o ${builddir}/${kernel} ${objs} ${libs}
	@printf "[ \e[32mLD\e[0m ]  %s\n" ${kernel}

.PHONY: iso
iso: link
	@mkdir -p ${isodir}/boot/grub
	@cp ${builddir}/${kernel} ${isodir}/boot/${kernel}
	@cp ${archdir}/grub.cfg ${isodir}/boot/grub/
	@sed 's/__kfs__/${kernel}/' ${archdir}/grub.cfg > ${isodir}/boot/grub/grub.cfg
	@${GRUBMK} ${GRUBMKFLAGS} -o ${builddir}/${kernel}.iso ${isodir}
	@printf "[ \e[34mMK\e[0m ]  %s\n" ${kernel}.iso

.PHONY: boot
boot:
	@${QEMU} ${QEMUFLAGS} -cdrom ${builddir}/${kernel}.iso

.PHONY: clean
clean: clean-subdir
	@if [ -d ${builddir} ]; then \
		for obj in $$(find . -type f -name '*.o' | tr '\n' ' '); do \
			if [ -f $$obj ]; then \
				${RM} $$obj; \
				printf "[ \e[31mRM\e[0m ]  %s\n" "$${obj#./build/}"; \
			fi; \
		done; \
		for obj in $$(find . -type f -name '*.a' | tr '\n' ' '); do \
			if [ -f $$obj ]; then \
				${RM} $$obj; \
				printf "[ \e[31mRM\e[0m ]  %s\n" "$${obj#./build/}"; \
			fi; \
		done; \
		${RM} $$(find . -type f -name '*.d'); \
		${RM} ${builddir}/${kernel}; \
		${RM} ${builddir}/${kernel}.iso; \
		${RM} -r ${isodir}; \
		printf "[ \e[31mRM\e[0m ]  %s\n" ${kernel}; \
		printf "[ \e[31mRM\e[0m ]  %s\n" ${kernel}.iso; \
		rmdir --ignore-fail-on-non-empty ${builddir}; \
	fi

.PHONY: fclean
fclean: clean
	@${RM} -r ${builddir}

.PHONY: re
re: clean all

# SUBDIR
.PHONY: build-subdir
build-subdir:
	@for subd in ${subdir}; do \
		${MAKE} -C $$subd builddir=${builddir} .INCLUDE_DIRS=${.INCLUDE_DIRS} all; \
	done

.PHONY: clean-subdir
clean-subdir:
	@for subd in ${subdir}; do \
		${MAKE} -C $$subd builddir=${builddir} .INCLUDE_DIRS=${.INCLUDE_DIRS} clean; \
	done
