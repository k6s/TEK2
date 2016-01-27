#include "malloc.h"

void				show_alloc_mem(void)
{
	t_chk_hdr		*chk;
	size_t			sz = 0;

	printf("break : 0x%04x\n", sbrk(0));
	if (g_arena.top)
	{
		sz += g_arena.top->size;
		chk = (void *)((uintptr_t)g_arena.top + CHK_HDR_SZ - g_arena.size);
		while (sz < g_arena.size)
		{
			printf("0x%04x - 0x%04x: 0x%04x\n", (uintptr_t)chk + CHK_HDR_SZ,
				   (uintptr_t)chk + chk->size, chk->size);
			sz += chk->size;
			chk = (uintptr_t)chk + chk->size;
		}
	}
}
