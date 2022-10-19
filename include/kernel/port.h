// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* port.h
 *
 * Interface to read and write to port.
 *
 * created: 2022/10/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef PORT_H
#define PORT_H

#include <stdint.h>

uint8_t		port_read(uint16_t port);
void		port_write(uint16_t port, uint8_t val);

/* Ports definition */

#define PS2_DATA 0x60
#define PS2_STATUS_REGISTER 0x64

#endif
