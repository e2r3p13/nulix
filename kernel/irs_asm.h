
#define IRS_ASM_H

#define ISR(name) \
.global name##_isr ;\
.section .text ;\
name##_isr: \
	cli ; \
	pusha ;\
	call name##_handler; \
	popa ;\
	iret

#endif
