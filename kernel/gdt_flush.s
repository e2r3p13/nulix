/* kernel/gdt_flush.s
 *
 * Flush the old gdt with a new one.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 */

.global gdt_flush
.section .text
gdt_flush:
	mov 4(%esp), %eax
	lgdt (%eax)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	jmp $0x08,$flush
flush:
	ret

