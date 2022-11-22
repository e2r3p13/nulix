// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* boot.s
 *
 * Boot sector of our kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/22 - mrxx0 <chcoutur@student.42.fr>
 */

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.global stack_bottom
.global stack_top

.section .bss
.align 16
stack_bottom:
	.skip 16384
stack_top:
	.section .text
	.global _start
	.type _start, @function
_start:
	mov $stack_top, %esp
	# Reset EFLAGS
	pushl $0
	popf
	# Push pointer to multiboot info struct
	pushl %ebx
	cli
	call kernel_main
	cli
1:	hlt
	jmp 1b
	.size _start, . - _start
