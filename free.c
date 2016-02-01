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

size_t				bin_lst_update(uintptr_t limit)
{
	t_chk_hdr		*chk;
	size_t			tfree;

	chk = g_arena.top->nxt;
	tfree = 0;
	while (chk)
	{
		if ((uintptr_t)chk + chk->size >= limit)
		{
			if (chk->nxt)
				chk->nxt->prv = chk->prv;
			if (chk->prv)
				chk->prv->nxt = chk->nxt;
			tfree += chk->size;
		}
		chk = chk->nxt;
	}
	return (tfree);
}

void				arena_dealloc(void)
{
	size_t			new_sz;
	size_t			used;
	size_t			top_chk_sz;

	used = g_arena.size - g_arena.top_un_sz - g_arena.top->size;
	new_sz = (used + PAGE_CACHE * PAGE_SIZE) / PAGE_SIZE * PAGE_SIZE;
	if (new_sz > g_arena.size)
	{
		printf("*** Arena size corruption. Aborting... ***\n");
		abort();
	}
	g_arena.top_un_sz -= bin_lst_update((uintptr_t)g_arena.top + new_sz);
	if (!brk((uintptr_t)g_arena.top + new_sz))
	{
		g_arena.size = new_sz;
		g_arena.top->size = g_arena.size - used - g_arena.top_un_sz;
	}
}

#include <signal.h>

void				segv_hdl(int signum)
{
	show_alloc_mem();
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
		signal(SIGSEGV, &segv_hdl);
		while (freed->nxt && freed->nxt != chk)
			freed = freed->nxt;
		if (freed->nxt == chk)
			double_free(ptr);
		else
		{
			freed->nxt = chk;
			chk->prv = freed;
			chk->nxt = (void *)0;
			if (!chk_is_highest(chk))
			{
				if ((g_arena.top_un_sz += chk->size) + g_arena.top->size > PAGE_CACHE * PAGE_SIZE)
					 arena_dealloc();
			}
		}
		signal(SIGSEGV, SIG_DFL);
	}
	pthread_mutex_unlock(&g_arena.lock);
}
