
# ****************
#	Variables:

SRCDIR	= kernel
INCDIR	= include
OBJDIR	= obj
ISODIR	= isodir

OBJS	=	$(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DPDCS	=	$(OBJS:.o=.d)
OBJCTT	=	$(shell ls $(OBJDIR)/*.o)
DPDCTT	=	$(shell ls $(OBJDIR)/*.d)

AS		= i686-elf-as
CC		= i686-elf-gcc

NAME	= kfs

SRCS	= kernel.c \

# ****************
#	Rules:

all: $(NAME)

$(NAME): $(OBJS)
	@$(AS) $(SRCDIR)/boot.s -o $(OBJDIR)/boot.o
	@printf "[\e[32mAS\e[0m] %s\n" boot.o
	@make -C lib/string
	@$(CC) -T linker.ld -o $(NAME).bin -ffreestanding -O2 -nostdlib $(OBJDIR)/boot.o $(OBJDIR)/kernel.o -lgcc lib/string/string.a -I include
	@printf "[\e[34mOK\e[0m] %s\n" $(NAME).bin

-include $(DPDCS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) -std=gnu99 -ffreestanding -O2 -Wall -Wextra -MMD -I $(INCDIR) -c $< -o $@
	@printf "[\e[32mCC\e[0m] %s\n" $@

iso: all
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(NAME).bin $(ISODIR)/boot/$(NAME).bin
	@cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	@grub2-mkrescue --compress=xz -o $(NAME).iso $(ISODIR)
	@printf "[\e[34mOK\e[0m] %s\n" $(NAME).iso
	
boot: iso
	@qemu-system-i386 -cdrom $(NAME).iso -curses


clean:
	@make clean -C lib/string
	rm -rf $(OBJDIR)
	rm -rf $(ISODIR)

fclean: clean
	@make fclean -C lib/string
	rm -f kfs.iso
	rm -f kfs.bin

