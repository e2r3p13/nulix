
cross-target:= i686-elf

ifndef ${builddir}
	.INCLUDE_DIRS= ../include
endif

# COMPILE VAR
AS:= ${cross-target}-as
ASFLAGS+=
AR:= ${cross-target}-ar
ARFLAGS:= rc
CC:= ${cross-target}-gcc
CFLAGS+= -Wall -Wextra -ffreestanding -nostdlib -nodefaultlibs -fno-builtin -mgeneral-regs-only -MMD $(addprefix -I, ${.INCLUDE_DIRS})
LD:= ${cross-target}-ld
LDFLAGS+=

# BUILD VAR
subdir:= \
		 keyboard \
		 memory \
		 nsh \

builddir?= build

src-y:= \
	kernel.c \
	boot_hhk.s \
	port.c \
	print.c \
	gdt.c \
	gdt_flush.s \
	idt.c \
	isr.c \
	spinlock.c \
	pic_8259.c \
	screenbuf.c \

objs:= $(addprefix ${builddir}/, ${src-y})
objs:= ${objs:.c=.o}
objs:= ${objs:.s=.o}

deps:= ${objs:.o=.d}
-include ${deps}

# RULES
.PHONY: all
all: build-subdir build

.PHONY: build
build: build-subdir ${objs}

${builddir}/%.o: %.c
	@mkdir -p ${builddir}
	@${CC} ${CFLAGS} -o $@ -c $<
	@printf "[ \e[32mCC\e[0m ]  %s\n" $<

${builddir}/%.o: %.s
	@mkdir -p ${builddir}
	@${AS} ${ASFLAGS} -o $@ -c $<
	@printf "[ \e[32mAS\e[0m ]  %s\n" $<

.PHONY: clean
clean: clean-subdir
	@${RM} ${deps}
	@if [ -d ${builddir} ]; then \
		for obj in ${objs}; do \
			if [ -f $$obj ]; then \
				${RM} $$obj; \
				printf "[ \e[31mRM\e[0m ]  %s\n" "$${obj#${builddir}/}"; \
			fi; \
		done; \
		rmdir --ignore-fail-on-non-empty ${builddir}; \
	fi

# SUBDIR
.PHONY: build-subdir
build-subdir:
	@for subd in ${subdir}; do \
		${MAKE} -C $$subd builddir=${builddir} all; \
	done

.PHONY: clean-subdir
clean-subdir:
	@for subd in ${subdir}; do \
		${MAKE} -C $$subd builddir=${builddir} clean; \
	done

