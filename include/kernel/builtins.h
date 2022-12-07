// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* builtins.h
 *
 * Builtins functions for shell.
 *
 * created: 2022/11/17 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/13 - glafond- <glafond-@student.42.fr>
 */

#ifndef BUILTINS_H
#define BUILTINS_H

void alloc(char **argv);
void free(char **argv);
void reboot(char **argv);
void poweroff(char **argv);
void key_us(char **argv);
void key_fr(char **argv);
void color_red(char **argv);
void color_reset(char **argv);
void clear(char **argv);
void help(char **argv);
void int_divide(char **argv);

#endif
