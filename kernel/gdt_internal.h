/* kernel/gdt_internal.h
 *
 * Define gdt structures.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef GDT_INTERNAL_H
#define GDT_INTERNAL_H

/* Describe an entry of the gdt.
 */
typedef struct {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
}__attribute__((packed)) t_gdt_entry;


/* Store the pointer to the gdt and its size.
 */
typedef struct {
	unsigned short limit;
	unsigned int base;
}__attribute__((packed)) t_gdt_ptr;

#endif
