// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* port.c
 *
 * Interface to read and write to port.
 *
 * created: 2022/10/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <stdint.h>

/* Wrapper to asm instruction 'in' - 1 byte
 *
 * @arg(port): port number to read on.
 * @ret: value read.
 */
uint8_t port_read_u8(uint16_t port) {
	uint8_t val;
	asm inline volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

/* Wrapper to asm instruction 'in' - 2 bytes
 *
 * @arg(port): port number to read on.
 * @ret: value read.
 */
uint16_t port_read_u16(uint16_t port) {
	uint16_t val;
	asm inline volatile ("inw %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

/* Wrapper to asm instruction 'out' - 1 byte
 *
 * @arg(port): port number to write on.
 * @arg(val): value to write.
 */
void port_write_u8(uint16_t port, uint8_t val) {
	asm inline volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Wrapper to asm instruction 'out' - 2 bytes
 *
 * @arg(port): port number to write on.
 * @arg(val): value to write.
 */
void port_write_u16(uint16_t port, uint16_t val) {
	asm inline volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}
