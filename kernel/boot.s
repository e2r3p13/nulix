// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* boot.s
 *
 * Boot sector of our kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/11 - xlmod <glafond-@student.42.fr>
 */

.global stack_top
.global stack_bottom

# Declare constants for the multiboot header.
.set ALIGN,    1 << 0			# align loaded modules on page boundaries
.set MEMINFO,  1 << 1			# provide memory map
.set FLAGS,    ALIGN | MEMINFO	# this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002		# 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)	# checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a temporary stack to use during pre paging step
.section .multiboot.bss, "aw"
.align 16
multiboot_stack_bottom:
	.skip 4096
multiboot_stack_top:

# Reserve a temporary stack to use before allocation of a new one
.section .bootstrap_stack, "aw",@nobits
stack_bottom:
	.skip 16384
stack_top:

.section .bss, "aw", @nobits
bss_top:
	.skip 4096
bss_bottom:

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:

	# Load temporary stack
	movl $multiboot_stack_top, %esp

	# Reset eflags
	pushl $0
	popf

	# Call boot_init that sets and loads a temporary page directory in cr3
	# and save %ebx that stores a pointer to the multiboot structure
	pushl %ebx
	cli
	call (boot_init - 0xc0000000)
	pop %ebx

	# Activate paging and protected mode
	movl %cr0, %ecx
	orl $0x80000001, %ecx
	movl %ecx, %cr0

	# Load the address of kernel_start and jump to the the effective address (high address)
	lea kernel_start, %ecx
	jmp *%ecx

.section .text
kernel_start:

	# Load the new temporary stack
	movl $stack_top, %esp

	# Call kernel c entry point with multibool structure in arguments
	pushl %ebx
	call kernel_init

1:
	cli
	hlt
	jmp 1b
