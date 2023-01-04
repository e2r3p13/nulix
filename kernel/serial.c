// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/serial.c
 *
 * Insert file description here
 *
 * created: 2022/11/29 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/04 - xlmod <glafond-@student.42.fr>
 */

#include <stddef.h>

#include <kernel/port.h>
#include <kernel/serial.h>

int serial_init() {
   port_write_u8(PORT + 1, 0x00);    // Disable all interrupts
   port_write_u8(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   port_write_u8(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   port_write_u8(PORT + 1, 0x00);    //                  (hi byte)
   port_write_u8(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   port_write_u8(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   port_write_u8(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   port_write_u8(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   port_write_u8(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty (i.e: not same byte as sent)
   if(port_read_u8(PORT + 0) != 0xAE) {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   port_write_u8(PORT + 4, 0x0F);
   return 0;
}

void serial_putchar(char a) {
   while ((port_read_u8(PORT + 5) & 0x20) == 0)
	   continue;
   port_write_u8(PORT, a);
}

void serial_putstr(char *s) {
	while (*s)
		serial_putchar(*s++);
}

void serial_write(char *s, size_t size) {
	while (size--)
		serial_putchar(*s++);
}
