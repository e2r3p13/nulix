

# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .multiboot.bss
.align 16
multiboot_stack_bottom:
	.skip 128
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

.section .multiboot.text
.global _start
.type _start, @function
_start:

	movl $multiboot_stack_top, %esp
	movl $(0xb8000), %edx
	movl $0x0f48, (%edx)
	hlt

.section .text
	hlt
