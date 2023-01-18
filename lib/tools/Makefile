
cross-target:= i686-elf

# COMPILE VAR
AS:= ${cross-target}-as
ASFLAGS+=
AR:= ${cross-target}-ar
ARFLAGS:= rc
CC:= ${cross-target}-gcc
CFLAGS+= -Wall -Wextra -ffreestanding -nostdlib -MMD $(addprefix -I, ${.INCLUDE_DIRS}) -mrdrnd
LD:= ${cross-target}-ld
LDFLAGS+=

# BUILD VAR
subdir:=

builddir?= build
local-builddir:= build

libname:= tools
src-y:= \
	crc32.c \
	random.c \

objs:= $(addprefix ${local-builddir}/, ${src-y})
objs:= ${objs:.c=.o}
objs:= ${objs:.s=.o}

deps:= ${objs:.o=.d}
-include ${defs}

# RULES
.PHONY: all
all: build lib

.PHONY: build
build: ${objs}

.PHONY: lib
lib: build
	@${AR} ${ARFLAGS} ${builddir}/lib${libname}.a ${objs}
	@printf "[ \e[32mAR\e[0m ]  %s\n" lib${libname}.a

${local-builddir}/%.o: %.c
	@mkdir -p ${local-builddir}
	@${CC} ${CFLAGS} -o $@ -c $<
	@printf "[ \e[32mCC\e[0m ]  %s\n" $<

${local-builddir}/%.o: %.s
	@mkdir -p ${local-builddir}
	@${AS} ${ASFLAGS} -o $@ -c $<
	@printf "[ \e[32mAS\e[0m ]  %s\n" $<

.PHONY: clean
clean:
	@${RM} ${deps}
	@if [ -d ${local-builddir} ]; then \
		for obj in ${objs}; do \
			if [ -f $(shell pwd)/$$obj ]; then \
				${RM} $$obj; \
				printf "[ \e[31mRM\e[0m ]  %s\n" "$${obj#${local-builddir}/}"; \
			fi; \
		done; \
		${RM} -r ${local-builddir}; \
	fi
	@if [ -d ${builddir} ]; then \
		if [ -f ${builddir}/lib${libname}.a ]; then \
			${RM} ${builddir}/lib${libname}.a; \
			printf "[ \e[31mRM\e[0m ]  %s\n" $(shell basename ${builddir}/lib${libname}.a); \
		fi; \
		rmdir --ignore-fail-on-non-empty ${builddir}; \
	fi
