// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt_internal.h
 *
 * Internal header that defines GDT structures
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/09 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef GDT_INTERNAL_H
#define GDT_INTERNAL_H

/* GDT entries information
 */
#define GDT_BASE_ADDRESS 0x800
#define GDT_SIZE 0x6
#define GDT_KERNEL_LIMIT 0xFFFF
#define GDT_USER_LIMIT 0xFFFF
#define GDT_GRAN_BASE 0xCF
#define GDT_ENTRY_NULL 0x0
#define GDT_KERNEL_CODE 0x1
#define GDT_KERNEL_DATA 0x2
#define GDT_KERNEL_STACK 0x3
#define GDT_USER_CODE 0x4
#define GDT_USER_DATA 0x5
#define GDT_USER_STACK 0x6
#define GDT_BASE_SEGMENT 0x0

/* GDT entries operations to retrieve segment value and segment access
 */
#define SEG_DESCTYPE(x) ((x) << 0x04)
#define SEG_PRES(x) ((x) << 0x07)
#define SEG_SAVL(x) ((x) << 0x0C)
#define SEG_LONG(x) ((x) << 0x0D)
#define SEG_SIZE(x) ((x) << 0x0E)
#define SEG_GRAN(x) ((x) << 0x0F)
#define SEG_PRIV(x) (((x)&0x03) << 0x05)
#define SEG_DATA_RD 0x00        // Read-Only
#define SEG_DATA_RDA 0x01       // Read-Only, accessed
#define SEG_DATA_RDWR 0x02      // Read/Write
#define SEG_DATA_RDWRA 0x03     // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04    // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05   // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06  // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08        // Execute-Only
#define SEG_CODE_EXA 0x09       // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A      // Execute/Read
#define SEG_CODE_EXRDA 0x0B     // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C       // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D      // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E     // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F    // Execute/Read, conforming, accessed

/* GDT_KERNEL_CODE_ACCESS - 0x9A */
#define GDT_KERNEL_CODE_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_CODE_EXRD

/* GDT_KERNEL_DATA_ACCESS - 0x92 */
#define GDT_KERNEL_DATA_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_DATA_RDWR

/* GDT_KERNEL_STACK_ACCESS - 0x96 */
#define GDT_KERNEL_STACK_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                          SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                          SEG_PRIV(0) | SEG_DATA_RDWREXPD

/* GDT_USER_CODE_ACCESS - 0xFA */
#define GDT_USER_CODE_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_CODE_EXRD

/* GDT_USER_DATA_ACCESS - 0xF2 */
#define GDT_USER_DATA_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_DATA_RDWR

/* GDT_USER_STACK_ACCESS - 0xF6 */
#define GDT_USER_STACK_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                          SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                          SEG_PRIV(3) | SEG_DATA_RDWREXPD

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
