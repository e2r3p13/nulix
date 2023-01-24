
.include "./registers_asm.s"

.macro ISR_NOCODE n
.global isr_\n
.section .text
isr_\n:
	cli
// save the stack
	push %ebp
	mov %esp, %ebp

REG_SAVE

	mov %esp, %ecx

	mov %ds, %ax
	push %eax
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov 8(%ebp), %eax
	and $0b11, %eax

	push %ecx // regs
	push $0
	push %eax // ring
	push $\n
	call isr_handler
	add $16, %esp

	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

REG_RESTORE

// restore the stack
	mov %ebp, %esp
	pop %ebp

	sti
	iret
.endm

.macro ISR_CODE n
.global isr_\n
.section .text
isr_\n:
	cli
	// retreving the error code
	push %eax
	mov 4(%esp), %eax
	mov %eax, -4(%esp)
	pop %eax

	// Removing the code from its previous location on the stack
	add $4, %esp

	// save the stack
	push %ebp
	mov %esp, %ebp

	// allocating the error code in the stack
	push -8(%esp)

REG_SAVE

	mov %esp, %ecx

	mov %ds, %ax
	push %eax
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov 8(%ebp), %eax
	and $0b11, %eax

	push %ecx // regs
	push (REGS_SIZE + 8)(%esp)
	push %eax // ring
	push $\n
	call isr_handler
	add $16, %esp

	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

REG_RESTORE

	add $4, %esp

// restore the stack
	mov %ebp, %esp
	pop %ebp

	sti
	iret
.endm

ISR_NOCODE 0
ISR_NOCODE 1
ISR_NOCODE 2
ISR_NOCODE 3
ISR_NOCODE 4
ISR_NOCODE 5
ISR_NOCODE 6
ISR_NOCODE 7
ISR_CODE 8
