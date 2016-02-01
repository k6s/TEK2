#include "malloc.h"

static void			double_free(void *ptr)
{
	fprintf(stderr, "*** Error: free(): Double free or corruption %p ***\n",
			ptr);
	show_alloc_mem();
	abort();
}

static int			is_ptr_valid(t_chk_hdr *ptr)
{
	if (IS_DISALIGNED((uintptr_t)ptr)
		|| ptr->size < 20 || ptr->size + BIN_HDR_SZ > g_arena.size)
	{
		fprintf(stderr, "*** Error: free(): Invalid pointer: %p ***\n",
				ptr);
		printf("%x\n", ptr->size);
		//show_alloc_mem();
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
	printf("free %p\n", ptr);
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
			memset(ptr, 0x62, chk->size - BIN_HDR_SZ);
		}
	}
	//show_alloc_mem();
	pthread_mutex_unlock(&g_arena.lock);
}
