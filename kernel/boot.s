
.global stack_top
.global stack_bottom

# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .multiboot.bss, "aw"
.align 16
multiboot_stack_bottom:
	.skip 4096
multiboot_stack_top:

.section .bootstrap_stack, "aw",@nobits
stack_bottom:
	.skip 16384
stack_top:

.section .bss, "aw", @nobits
.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:

	movl $multiboot_stack_top, %esp

# Reset eflags
	pushl $0
	popf

	pushl %ebx
	cli
	call (boot_init - 0xc0000000)
	pop %ebx

	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	lea 4f, %ecx
	jmp *%ecx


.section .text
4:
	movl $stack_top, %esp
	pushl %ebx
	call kernel_main

	hlt
