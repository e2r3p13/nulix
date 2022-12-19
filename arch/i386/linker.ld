
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

	.text ALIGN (4K) : AT (ADDR (.text) - kernel_virt_offset) {
		sym_start_text = . - kernel_virt_offset;
		*(.text)
		sym_end_text = . - kernel_virt_offset;
	}

	.rodata ALIGN (4K) : AT (ADDR (.rodata) - kernel_virt_offset) {
		sym_start_rodata = . - kernel_virt_offset;
		*(.rodata)
		sym_end_rodata = . - kernel_virt_offset;
	}

	.data ALIGN (4K) : AT (ADDR (.data) - kernel_virt_offset) {
		sym_start_data = . - kernel_virt_offset;
		*(.data)
		sym_end_data = . - kernel_virt_offset;
	}

	.bss ALIGN (4K) : AT (ADDR (.bss) - kernel_virt_offset) {
		sym_start_bss = . - kernel_virt_offset;
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		sym_end_bss = . - kernel_virt_offset;
	}

	ek = . - kernel_virt_offset;

}
