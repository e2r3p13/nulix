// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/nsh.h
 *
 * Nulix shell header file
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/14 - glafond- <glafond-@student.42.fr>
 */

#ifndef NSH_H
#define NSH_H

#define NSH_BUFSIZE 1024

struct builtin {
	char *name;
	int (*exec)(int ,char **);
	char *description;
};

void nsh();

#endif
