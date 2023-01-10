
ENTRY (_start)

SECTIONS
{
	kernel_virt_offset = 0xC0000000;
	. = 0x00100000;

	sym_kernel_start = .;

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
	sym_kernel_virt_start = .;

	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000) {
		sym_virt_text = .;
		sym_text = . - kernel_virt_offset;
		*(.text)
	}

	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000) {
		sym_virt_rodata = .;
		sym_rodata = . - kernel_virt_offset;
		*(.rodata)
	}

	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000) {
		sym_virt_data = .;
		sym_data = . - kernel_virt_offset;
		*(.data)
	}

	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000) {
		sym_virt_bss = .;
		sym_bss = . - kernel_virt_offset;
		*(COMMON)
		*(.bss)
	}

	tmp0 = .;
	tmp_stack_virt = 0xffbfc000;
	. += tmp_stack_virt - tmp0;
	.stack ALIGN (4K) : AT (ADDR (.stack) - (kernel_virt_offset + (tmp_stack_virt - tmp0))) {
		sym_virt_stack = .;
		sym_stack = . - (tmp_stack_virt - tmp0) - kernel_virt_offset;
		*(.bootstrap_stack)
	}
	. -= tmp_stack_virt - tmp0;

	.buddy ALIGN (4K) : AT (ADDR (.buddy) - 0xC0000000) {
		sym_virt_buddy = .;
		sym_buddy = . - kernel_virt_offset;
		*(.buddy)
	}
	sym_buddy_size = . - sym_virt_buddy;

	.heap ALIGN (2M) : AT (ADDR (.heap) - 0xC0000000) {
		sym_virt_heap = .;
		sym_heap = . - kernel_virt_offset;
		*(.heap)
	}

	sym_kernel_virt_end = .;
	. -= kernel_virt_offset;
	sym_kernel_end = .;
}
