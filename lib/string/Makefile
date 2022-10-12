# SPDX-FileCopyrightText: 2022 lfalkau
# SPDX-License-Identifier: Apache-2.0

# ****************
#	Variables:

LIBNAME	=	string.a

AR		=	i686-elf-ar
ARFLAGS	=	rc

CC		=	i686-elf-gcc
CFLAGS	=	-Wall -Wextra -Werror

INCDIR	=	../../include
OBJDIR	=	obj

SRCS	=	str.c \
			mem.c \

OBJS	=	$(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DPDCS	=	$(OBJS:.o=.d)

OBJDIROBJCONTENT	=	$(shell ls $(OBJDIR)/*.o)
OBJDIRDPDCSCONTENT	=	$(shell ls $(OBJDIR)/*.d)

# ****************
#	Rules:

all: $(LIBNAME)

$(LIBNAME): $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBNAME) $(OBJS)
	@printf "[\e[32mOK\e[0m] %s\n" $@

-include $(DPDCS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) $(FSAN) -MMD -I$(INCDIR) -c $< -o $@
	@printf "[\e[32mCC\e[0m] %s\n" $@

clean: _clean

fclean: _clean
ifeq ($(shell ls -1 | grep $(LIBNAME)),$(LIBNAME))
	@rm -rf $(LIBNAME)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(LIBNAME)
endif

_clean:
ifeq ($(shell ls -1 | grep $(OBJDIR)),$(OBJDIR))
	@rm -rf $(OBJDIROBJCONTENT) $(OBJDIRDPDCSCONTENT)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(OBJDIROBJCONTENT)
	@rm -rf $(OBJDIR)
endif

re: fclean all

