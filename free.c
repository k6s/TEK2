#include "malloc.h"
#include <stdio.h>

void				free(void *ptr)
{
	t_chk_hdr		*wilderness;
	t_chk_hdr		*chk;
	t_chk_hdr		*freed;

	if (!ptr)
		return ;
	printf("FREEING %p\n", ptr);
	chk = (t_chk_hdr *)((uintptr_t)ptr - CHK_HDR_SZ);
	wilderness = (t_chk_hdr *)((uintptr_t)sbrk(0) - CHK_WILD_OFF);
	if ((freed = wilderness->nxt))
	{
		while (freed->nxt)
			freed = freed->nxt;
		freed->nxt = chk;
		chk->prv = freed;
	}
	else
	{
		wilderness->nxt = chk;
		chk->prv = wilderness;
	}
	chk->nxt = (void *)0;
}
