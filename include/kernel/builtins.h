// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* builtins.h
 *
 * Builtins functions for shell.
 *
 * created: 2022/11/17 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/13 - glafond- <glafond-@student.42.fr>
 * updated: 2022/12/08 - xlmod <glafond-@student.42.fr>
 */

#ifndef BUILTINS_H
#define BUILTINS_H

int color(int argc, char **argv);

int reboot(int argc, char **argv);
int poweroff(int argc, char **argv);

int keymap(int argc, char **argv);

int alloc(int argc, char **argv);
int free(int argc, char **argv);

//int clear(int argc, char **argv);
//int help(int argc, char **argv);
//int int_divide(int argc, char **argv);

#endif
