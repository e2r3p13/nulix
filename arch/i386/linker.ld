
ENTRY (_start)

SECTIONS
{
	kernel_virt_offset = 0xC0000000;
	. = 0x00100000;

	sk = .;

	.multiboot.data : {
		*(.multiboot.data)
	}

   .multiboot.text : {
	   *(.multiboot.text)
   }

	.multiboot.bss : {
		*(.multiboot.bss)
	}

	. += kernel_virt_offset;

	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000) {
		*(.text)
	}

	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000) {
		*(.rodata)
	}

	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000) {
		*(.data)
	}

	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000) {
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	.buddy ALIGN (4K) : AT (ADDR (.buddy) - 0xC0000000) {
		*(.buddy)
	}

	.heap ALIGN (4K) : AT (ADDR (.heap) - 0xC0000000) {
		sym_heap = .;
		*(.heap)
	}

	ek = .;
	/*
	. += 0xffbff000 - ek;

	.stack ALIGN (4K) : AT (ADDR (.stack) - (0xC0000000 + (0xffbff000 - ek))) {
	}
		*/

}
