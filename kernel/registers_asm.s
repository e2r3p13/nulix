
.set REGS_SIZE, 128

.macro REG_SAVE
	// reserved on stack
	sub $REGS_SIZE, %esp

	// copy register
	// segments registers
	movl $0,  0x00(%esp)
	mov %ss, 0x00(%esp)
	movl $0,  0x04(%esp)
	mov %cs, 0x04(%esp)
	movl $0,  0x08(%esp)
	mov %ds, 0x08(%esp)
	movl $0,  0x0c(%esp)
	mov %es, 0x0c(%esp)
	movl $0,  0x10(%esp)
	mov %fs, 0x10(%esp)
	movl $0,  0x14(%esp)
	mov %gs, 0x14(%esp)

	// general registers
	mov %esi, 0x28(%esp)
	mov %edi, 0x2c(%esp)
	mov %eax, 0x30(%esp)
	mov %ebx, 0x34(%esp)
	mov %ecx, 0x38(%esp)
	mov %edx, 0x3c(%esp)

	mov %ebp, %eax
	add $16, %eax
	mov %eax, 0x1c(%esp)
	mov (%ebp), %eax
	mov %eax, 0x20(%esp)

	// eflags
	pushf
	pop %eax
	mov %eax, 0x18(%esp)

	// eip
	mov 4(%ebp), %eax
	movl %eax, 0x24(%esp)

.endm

.macro REG_RESTORE

	// segments registers
	mov 0x10(%esp), %fs 
	mov 0x14(%esp), %gs 

	// general registers
	mov 0x28(%esp), %esi
	mov 0x2c(%esp), %edi
	mov 0x30(%esp), %eax
	mov 0x34(%esp), %ebx
	mov 0x38(%esp), %ecx
	mov 0x3c(%esp), %edx
	
	add $REGS_SIZE, %esp

.endm
