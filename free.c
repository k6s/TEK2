#include "malloc.h"

static void			double_free(void *ptr)
{
	fprintf(stderr, "*** Error: free(): Double free or corruption %p ***\n",
			ptr);
	abort();
}

static int			is_ptr_valid(t_chk_hdr *ptr)
{
	if (IS_DISALIGNED((uintptr_t)ptr)
		|| ptr->size < 20 || ptr->size + CHK_HDR_SZ > g_arena.size)
	{
		fprintf(stderr, "*** Error: free(): Invalid pointer: %p ***\n",
				ptr);
		printf("%x\n", ptr->size);
		show_alloc_mem();
		abort();
		return (-1);
	}
	return (0);
}
/*
static int			arena_dealloc(size_t size)
{
	size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
	if (brk(g_arena.top + BIN_HDR_SZ - g_arena.size + size))
		return (-1);
	g_arena.top->
	return (0);
}
*/
void				free(void *ptr)
{
	t_chk_hdr		*chk;
	t_chk_hdr		*freed;

	if (!ptr)
		return ;
	pthread_mutex_lock(&g_arena.lock);
	chk = (void *)((uintptr_t)ptr - BIN_HDR_SZ);
	if (!is_ptr_valid(chk))
	{
		freed = g_arena.top;
		while (freed->nxt && freed->nxt != chk)
			freed = freed->nxt;
		if (freed->nxt == chk)
			double_free(ptr);
		else
		{
			freed->nxt = chk;
			chk->prv = freed;
			chk->nxt = (void *)0;
			if ((uintptr_t)chk + chk->size == (uintptr_t)g_arena.top + BIN_HDR_SZ
				- g_arena.top->size)
			{
				if ((g_arena.top_un_sz += chk->size) > PAGE_SIZE * PAGE_CACHE)
			//		arena_resize(g_arena.size - (g_arena.top_un_sz)
			//					 + PAGE_SIZE * PAGE_CACHE);
				printf("Unused at top: %u\n", g_arena.top_un_sz);
			}
		}
	}
	pthread_mutex_unlock(&g_arena.lock);
}
