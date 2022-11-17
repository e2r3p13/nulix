// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* PS/2 controller header file
 *
 * Mainly contains port definitions
 *
 * created: 2022/11/04 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/04 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef PS_2_H
#define PS_2_H

/* Ports definition */

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_REGISTER_PORT 0x64
#define PS2_STATUS_COMMAND_PORT 0x64

/* Status register flags */

#define PS2_OUTPUT_BUFFER_FULL 0x1
#define PS2_INPUT_BUFFER_FULL 0x2

/* Commands codes */

#define CPU_RESET 0xFE

#endif
