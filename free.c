#include "malloc.h"

void				free(void *ptr)
{
	t_chk_hdr		*chk;
	t_chk_hdr		*freed;

	if (!ptr)
		return ;
//	printf("freeing %p\n", ptr);
	chk = (void *)((uintptr_t)ptr - CHK_HDR_SZ);
	freed = g_arena.top;
	while (freed->nxt)
		freed = freed->nxt;
	freed->nxt = chk;
	chk->prv = freed;
	chk->nxt = (void *)0;
//	if (ptr == 0x422c608)
//		show_alloc_mem();
}
