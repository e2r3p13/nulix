// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/power.c
 *
 * Power builtin file
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/12 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/port.h>
#include <kernel/ps2.h>
#include <kernel/print.h>

/*
 * Reboot kernel through the PS/2 controller by sending
 * a reset signal to the CPU as soon as the ps/2 data
 * buffer is empty.
 */
int reboot(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char **argv) {
	klog(SERIAL_KINFO "REBOOT\n\n");
	while (port_read_u8(PS2_STATUS_REGISTER_PORT) & PS2_INPUT_BUFFER_FULL);
	port_write_u8(PS2_STATUS_COMMAND_PORT, CPU_RESET);
	asm volatile ("hlt");
	return 0;
}

/*
 * Poweroff the kernel through the qemu debug port
 */
int poweroff(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char **argv) {
	klog(SERIAL_KINFO "POWEROFF\n\n");
	port_write_u16(0x604, 0x2000);
	asm volatile ("hlt");
	return 0;
}
