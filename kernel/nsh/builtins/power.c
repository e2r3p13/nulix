// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/power.c
 *
 * Insert file description here
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/port.h>
#include <kernel/ps2.h>

/*
 * Reboot kernel through the PS/2 controller by sending
 * a reset signal to the CPU as soon as the ps/2 data
 * buffer is empty.
 */
int reboot(int argc, char **argv) {
	while (port_read_u8(PS2_STATUS_REGISTER_PORT) & PS2_INPUT_BUFFER_FULL);
	port_write_u8(PS2_STATUS_COMMAND_PORT, CPU_RESET);
	asm volatile ("hlt");
	return 0;
}

/*
 * Poweroff the kernel through the qemu debug port
 */
int poweroff(int argc, char **argv) {
	port_write_u16(0x604, 0x2000);
	asm volatile ("hlt");
	return 0;
}
