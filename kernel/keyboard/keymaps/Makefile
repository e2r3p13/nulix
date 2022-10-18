
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
CFLAGS+= -ffreestanding -nostdlib -MMD $(addprefix -I, ${.INCLUDE_DIRS})
LD:= ${cross-target}-ld
LDFLAGS+=

# BUILD VAR
subdir:=

builddir?= build

src-y:= \
	US104.c \
	FR.c \

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
	@${RM} ${objs}
	@${RM} ${deps}
	@rmdir --ignore-fail-on-non-empty ${builddir}
	@for obj in $(foreach o,${objs},$(shell basename ${o})); do \
		printf "[ \e[31mRM\e[0m ]  %s\n" $$obj; \
	done

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

