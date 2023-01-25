
.include "./registers_asm.s"

.macro ISR_NOCODE n
.global isr_\n
.section .text
isr_\n:
	cli
// save the stack
	pushl %ebp
	movl %esp, %ebp

REG_SAVE

	movl %esp, %ecx

	movw %ds, %ax
	pushl %eax
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movl 8(%ebp), %eax
	and $0b11, %eax

	pushl %ecx // regs
	pushl $0
	pushl %eax // ring
	pushl $\n
	call isr_handler
	add $16, %esp

	popl %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

REG_RESTORE

// restore the stack
	movl %ebp, %esp
	popl %ebp

	sti
	iret
.endm

.macro ISR_CODE n
.global isr_\n
.section .text
isr_\n:
	cli
	// retreving the error code
	pushl %eax
	movl 4(%esp), %eax
	movl %eax, -4(%esp)
	popl %eax

	// Removing the code from its previous location on the stack
	add $4, %esp

	// save the stack
	pushl %ebp
	movl %esp, %ebp

	// allocating the error code in the stack
	pushl -8(%esp)

REG_SAVE

	movl %esp, %ecx

	movw %ds, %ax
	pushl %eax
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movl 8(%ebp), %eax
	and $0b11, %eax

	pushl %ecx // regs
	pushl (REGS_SIZE + 8)(%esp)
	pushl %eax // ring
	pushl $\n
	call isr_handler
	add $16, %esp

	popl %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

REG_RESTORE

	add $4, %esp

// restore the stack
	movl %ebp, %esp
	popl %ebp

	sti
	iret
.endm

.macro IRQ n
.global irq_\n
.section .text
irq_\n:
	cli
// save the stack
	pushl %ebp
	movl %esp, %ebp

REG_SAVE

	movl %esp, %ecx

	movw %ds, %ax
	pushl %eax
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movl 8(%ebp), %eax
	and $0b11, %eax

	pushl %ecx // regs
	pushl $0
	pushl %eax // ring
	pushl $(\n + 0x20)
	call isr_handler
	add $16, %esp

	pushl $\n
	call pic_8259_eoi
	add $4, %esp

	popl %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

REG_RESTORE

// restore the stack
	movl %ebp, %esp
	popl %ebp

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
ISR_NOCODE 9
ISR_CODE 10
ISR_CODE 11
ISR_CODE 12
ISR_CODE 13
ISR_CODE 14
ISR_NOCODE 16
ISR_CODE 17
ISR_NOCODE 18
ISR_NOCODE 19
ISR_NOCODE 20

IRQ 0
IRQ 1
