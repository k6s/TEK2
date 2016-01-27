#include "malloc.h"

void				show_alloc_mem(void)
{
	t_chk_hdr		*chk;
	t_heap_hdr		*heap;
	size_t			sz = 0;

	if ((heap = sbrk(0)) != (void *)-1)
	{
		printf("break : 0x%04x\n", heap);
		heap = (uintptr_t)heap - HEAP_HDR_SZ;
		chk = (uintptr_t)heap - CHK_HDR_SZ;
		sz += chk->size;
		printf("%x %x\n", heap->size, chk->size);
		chk = (uintptr_t)heap + HEAP_HDR_SZ - heap->size;
		while (sz < heap->size)
		{
			printf("0x%04x - 0x%04x: 0x%04x\n", (uintptr_t)chk + CHK_HDR_SZ,
				   (uintptr_t)chk + chk->size, chk->size);
			sz += chk->size;
			chk = (uintptr_t)chk + chk->size;
		}
	}
}
