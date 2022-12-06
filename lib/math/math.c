// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* mem.c
 *
 * memory related functions of the string library
 *
 * created: 2022/12/06 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/06 - mrxx0 <chcoutur@student.42.fr>
 */

int sqrt(int n)
{
	int i = 1;
	while (i * i < n)
		i++;
	if (i * i == n)
		return i;
	return -1;
}

int pow(int base, int exp)
{
	int i = base;
	while (exp)
	{
		base = base * i;
		exp--;
	}
	return base;
}

int log2(int n)
{
	int i = 1;
	int res = 2;

	while (n >= 2 && res < n)
	{
		res = res * 2;
		i++;
	}
	if (res == n)
		return i;
	return -1;
}
 
int log10(int n)
{
	int i = 1;
	int res = 10;

	while (n >= 10 && res < n)
	{
		res = res * 10;
		i++;
	}
	if (res == n)
		return i;
	return -1;
}
