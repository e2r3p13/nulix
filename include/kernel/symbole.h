// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/symbole.h
 *
 * extern symbole of the kernel
 *
 * created: 2023/01/10 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/11 - xlmod <glafond-@student.42.fr>
 */

#ifndef SYMBOLE_H
#define SYMBOLE_H

extern uintptr_t sym_kernel_start;
#define SYM_KERNEL_START ((uintptr_t)&sym_kernel_start)
extern uintptr_t sym_kernel_virt_start;
#define SYM_KERNEL_VIRT_START ((uintptr_t)&sym_kernel_virt_start)

extern uintptr_t sym_kernel_end;
#define SYM_KERNEL_END ((uintptr_t)&sym_kernel_end)
extern uintptr_t sym_kernel_virt_end;
#define SYM_KERNEL_VIRT_END ((uintptr_t)&sym_kernel_virt_end)

extern uintptr_t sym_text;
#define SYM_TEXT ((uintptr_t)&sym_text)
extern uintptr_t sym_virt_text;
#define SYM_VIRT_TEXT ((uintptr_t)&sym_virt_text)

extern uintptr_t sym_rodata;
#define SYM_RODATA ((uintptr_t)&sym_rodata)
extern uintptr_t sym_virt_rodata;
#define SYM_VIRT_RODATA ((uintptr_t)&sym_virt_rodata)

extern uintptr_t sym_data;
#define SYM_DATA ((uintptr_t)&sym_data)
extern uintptr_t sym_virt_data;
#define SYM_VIRT_DATA ((uintptr_t)&sym_virt_data)

extern uintptr_t sym_bss;
#define SYM_BSS ((uintptr_t)&sym_bss)
extern uintptr_t sym_virt_bss;
#define SYM_VIRT_BSS ((uintptr_t)&sym_virt_bss)

extern uintptr_t sym_buddy;
#define SYM_BUDDY ((uintptr_t)&sym_buddy)
extern uintptr_t sym_virt_buddy;
#define SYM_VIRT_BUDDY ((uintptr_t)&sym_virt_buddy)
extern uintptr_t sym_buddy_size;
#define SYM_BUDDY_SIZE ((uintptr_t)&sym_buddy_size)

extern uintptr_t sym_heap;
#define SYM_HEAP ((uintptr_t)&sym_heap)
extern uintptr_t sym_virt_heap;
#define SYM_VIRT_HEAP ((uintptr_t)&sym_virt_heap)

extern uintptr_t sym_stack;
#define SYM_STACK ((uintptr_t)&sym_stack)
extern uintptr_t sym_virt_stack;
#define SYM_VIRT_STACK ((uintptr_t)&sym_virt_stack)

extern uintptr_t stack_top;
#define STACK_TOP ((uintptr_t)&stack_top)
extern uintptr_t stack_bottom;
#define STACK_BOTTOM ((uintptr_t)&stack_bottom)

#endif
