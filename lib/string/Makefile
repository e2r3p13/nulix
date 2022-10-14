
cross-target:= i686-elf

# COMPILE VAR
AS:= ${cross-target}-as
ASFLAGS+=
AR:= ${cross-target}-ar
ARFLAGS:= rc
CC:= ${cross-target}-gcc
CFLAGS+= -ffreestanding -nostdlib -MMD $(addprefix -I, ${.INCLUDE_DIRS})
LD:= ${cross-target}-ld
LDFLAGS+=

# BUILD VAR
subdir:=

builddir?= build
local-builddir:= build

libname:= string
src-y:= str.c mem.c

objs:= $(addprefix ${local-builddir}/, ${src-y})
objs:= ${objs:.c=.o}
objs:= ${objs:.s=.o}

deps:= ${objs::o=.b}
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
	@${RM} ${objs}
	@${RM} ${builddir}/lib${libname}.a
	@${RM} -r ${local-builddir}
	@for obj in $(foreach o,${objs},$(shell basename ${o})); do \
		printf "[ \e[31mRM\e[0m ]  %s\n" $$obj; \
	done
	@printf "[ \e[31mRM\e[0m ]  %s\n" $(shell basename ${builddir}/lib${libname}.a)
