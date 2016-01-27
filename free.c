#include "malloc.h"

void				free(void *ptr)
{
	t_chk_hdr		*chk;
	t_chk_hdr		*freed;

	if (!ptr)
		return ;
	chk = (void *)((uintptr_t)ptr - CHK_HDR_SZ);
	if ((freed = g_arena.top->nxt))
	{
		while (freed->nxt)
			freed = freed->nxt;
		freed->nxt = chk;
		chk->prv = freed;
	}
	else
	{
		g_arena.top->nxt = chk;
		chk->prv = g_arena.top;
	}
	chk->nxt = (void *)0;
}
