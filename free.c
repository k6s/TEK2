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
		abort();
		return (-1);
	}
	return (0);
}

void				free(void *ptr)
{
	t_chk_hdr		*chk;
	t_chk_hdr		*freed;

	if (!ptr)
		return ;
	pthread_mutex_lock(&g_arena.lock);
	chk = (void *)((uintptr_t)ptr - CHK_HDR_SZ);
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
		}
	}
	pthread_mutex_unlock(&g_arena.lock);
}
