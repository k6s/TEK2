#include "malloc.h"

void				show_alloc_mem(void)
{
	t_chk_hdr		*chk;
	size_t			sz = 0;

	printf("break : 0x%04lx %p\n", (uintptr_t)sbrk(0), g_arena.top);
	if (g_arena.top)
	{
		sz += g_arena.top->size;
		chk = (void *)((uintptr_t)g_arena.top + BIN_HDR_SZ);
		while (sz < g_arena.size)
		{
			printf("0x%p - 0x%p: 0x%04lx\n", chk,
				   (uintptr_t)chk + chk->size, chk->size);
			sz += chk->size;
			if (!chk->size && sz < g_arena.size)
			{
				printf("HEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEERE\n");
				break ;
			}
			chk = (void *)((uintptr_t)chk + chk->size);
	}}
	chk = g_arena.top->nxt;
	printf("Free Bins List: %p\n", g_arena.top->nxt);
	while (chk)
	{
		printf("0x%04lx - 0x%04lx: 0x%04lx\n", (uintptr_t)chk,
			   (uintptr_t)chk + chk->size, chk->size);
		chk = chk->nxt;
	}
}
