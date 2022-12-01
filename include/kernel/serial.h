// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/serial.h
 *
 * Insert file description here
 *
 * created: 2022/11/29 - xlmod <glafond-@student.42.fr>
 * updated: 2022/11/29 - xlmod <glafond-@student.42.fr>
 */

#ifndef SERIAL_H
#define SERIAL_H

#define PORT 0x3f8

int serial_init();
void serial_write(char *s, size_t size);
void serial_write_str(char *s);

#endif
