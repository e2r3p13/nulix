
cross-target:= i686-elf-
target:= i386

kernel-name:= kfs
kernel-version:= 0.0.1
kernel:= ${kernel-name}-${kernel-version}

#BUILD 
archdir:= arch/${target}
incdir:= include
builddir:= build
isodir:= ${builddir}/iso
src-dir:= kernel drivers lib

src-c:= $(shell find $(src-dir) -type f -regex ".*\.c")
src-s:= $(shell find $(src-dir) -type f -regex ".*\.s")

objs:= $(addprefix $(builddir)/, $(patsubst %.c, %.o, $(src-c)))
objs+= $(addprefix $(builddir)/, $(patsubst %.s, %.o, $(src-s)))

# COMPILE VAR
AS:= ${cross-target}as
ASFLAGS+=
CC:= ${cross-target}gcc
CFLAGS+= -Wall -Wextra
CFLAGS+= -ffreestanding -nostdlib -nodefaultlibs -fno-builtin
CFLAGS+= -mgeneral-regs-only -MMD
CFLAGS+= $(addprefix -I, ${.INCLUDE_DIRS})
LD:= ${cross-target}ld
LDFLAGS+= -T ${archdir}/linker.ld

GRUBMK:=grub2-mkrescue
GRUBMKFLAGS+=--compress=xz

QEMU:= qemu-system-x86_64
QEMU_MEM:= -m 128M
QEMU_DEBUG:=
QEMUFLAGS+= -serial mon:stdio ${QEMU_DEBUG} ${QEMU_MEM}

.PHONY: all
all: build link

.PHONY: build
build: ${objs}

.PHONY: link
link: build
	@${LD} ${LDFLAGS} -o ${builddir}/${kernel} ${objs}
	@printf "[ \e[32mLD\e[0m ]  %s\n" ${kernel}
	
.PHONY: run
run: link
	@${QEMU} ${QEMUFLAGS} -kernel ${builddir}/${kernel}

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
clean:
	@if [ -d ${builddir} ]; then \
		for obj in $$(find . -type f -name '*.o' | tr '\n' ' '); do \
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

${builddir}/%.o: %.c
	@mkdir -p $(builddir)/$(dir $<)
	@${CC} ${CFLAGS} -o $@ -c $<
	@printf "[ \e[32mCC\e[0m ]  %s\n" $<

${builddir}/%.o: %.s
	@mkdir -p $(builddir)/$(dir $<)
	@${AS} ${ASFLAGS} -o $@ -c $<
	@printf "[ \e[32mAS\e[0m ]  %s\n" $<

