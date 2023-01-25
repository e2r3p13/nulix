
.set REGS_SIZE, 140

.macro REG_SAVE
	// reserved on stack
	sub $REGS_SIZE, %esp

	// copy register
	// segments registers
	movl $0,  0x00(%esp)
	movw %ss, 0x00(%esp)
	movl $0,  0x04(%esp)
	movw %cs, 0x04(%esp)
	movl $0,  0x08(%esp)
	movw %ds, 0x08(%esp)
	movl $0,  0x0c(%esp)
	movw %es, 0x0c(%esp)
	movl $0,  0x10(%esp)
	movw %fs, 0x10(%esp)
	movl $0,  0x14(%esp)
	movw %gs, 0x14(%esp)

	// general registers
	movl %esi, 0x28(%esp)
	movl %edi, 0x2c(%esp)
	movl %eax, 0x30(%esp)
	movl %ebx, 0x34(%esp)
	movl %ecx, 0x38(%esp)
	movl %edx, 0x3c(%esp)

	mov %cr0, %eax
	mov %eax, 0x40(%esp)
	mov %cr2, %eax
	mov %eax, 0x44(%esp)
	mov %cr3, %eax
	mov %eax, 0x48(%esp)

	movl %ebp, %eax
	add $16, %eax
	movl %eax, 0x1c(%esp)
	movl (%ebp), %eax
	movl %eax, 0x20(%esp)

	// eflags
	movl 12(%ebp), %eax
	movl %eax, 0x18(%esp)

	// eip
	movl 4(%ebp), %eax
	movl %eax, 0x24(%esp)

.endm

.macro REG_RESTORE

	// segments registers
	movw 0x10(%esp), %fs 
	movw 0x14(%esp), %gs 

	// general registers
	movl 0x28(%esp), %esi
	movl 0x2c(%esp), %edi
	movl 0x30(%esp), %eax
	movl 0x34(%esp), %ebx
	movl 0x38(%esp), %ecx
	movl 0x3c(%esp), %edx
	
	add $REGS_SIZE, %esp

.endm
