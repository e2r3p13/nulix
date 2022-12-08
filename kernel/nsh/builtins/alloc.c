// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/alloc.c
 *
 *
 *
 * created: 2022/12/13 - glafond- <glafond-@student.42.fr>
 * updated: 2022/12/13 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>

kpm_chunk_t g_chunk;

uint32_t pseudorand(uint32_t min, uint32_t max) {
	static uint32_t m_seed = 42;

    // Constants for the LCG
    const uint32_t a = 1103515245;
    const uint32_t c = 12345;
    const uint32_t m = (uint32_t)(1 << 16) + 1;

    // Generate the next pseudo-random number
    m_seed = (a * m_seed + c) % m;

    // Scale the random number to the desired range and return it
    return (m_seed % (max - min + 1)) + min;
}

/* Choose a pseudo random size, print it,
 * allocates a chunk of this size with kpm_alloc
 * and print the resulting chunk.
 */
void alloc() {
	size_t size;
	int res;

	size = pseudorand(1, 1 << 16);
	sb_set_fg(sb + sb_index, VGA_COLOR_GREEN);
	kprintf("Asked for allocation of %u bytes\n", size);
	res = kpm_alloc(&g_chunk, size);
	kprintf("status code: %d\n", res);
	sb_set_fg(sb + sb_index, VGA_DFL_COLOR);
	kprintf("chunk: {\n    addr = %p\n    size = %u\n}\n", g_chunk.addr, g_chunk.size);
}

void free() {
	kpm_free(&g_chunk);
}

