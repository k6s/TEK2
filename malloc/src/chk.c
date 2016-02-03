#include "malloc.h"

size_t					to_chk_size(size_t size)
{
	size = ALIGN(size + CHK_HDR_SZ);
	if (size < BIN_HDR_SZ)
		size = ALIGN(BIN_HDR_SZ);
	return (size);
}

int						chk_is_highest(t_chk_hdr *chk)
{
	if ((uintptr_t)chk + chk->size
		== (uintptr_t)g_arena.top + g_arena.size + BIN_HDR_SZ
		- g_arena.top->size)
		return (0);
	return (-1);
}

void					 chk_is_valid(t_chk_hdr *chk, size_t off)
{
	if ((!chk->size && off < g_arena.size)
		|| chk->size > g_arena.size)
	{
		fprintf(stdout, "*** Error: is_chk_valid(): Invalid next \
chunk %p ***\n", (void *)((uintptr_t)chk + chk->size));
		abort();
	}
}
