// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/registers.h
 *
 * Registers header
 *
 * created: 2023/01/24 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/24 - glafond- <glafond-@student.42.fr>
 */

#ifndef REGISTERS_H
#define REGISTERS_H

struct registers {
	uint32_t ss;
	uint32_t cs;
	uint32_t ds;
	uint32_t es;
	uint32_t fs;
	uint32_t gs;
	uint32_t flg;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eip;
	uint32_t esi;
	uint32_t edi;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} __attribute__ ((packed));

#endif
