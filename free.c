#include "malloc.h"

static void __attribute__((noreturn))	double_free(void *ptr)
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

static size_t			bin_lst_update(uintptr_t limit)
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

	used = g_arena.size - g_arena.top_un_sz - g_arena.top->size;
	new_sz = (used + PAGE_CACHE * PAGE_SIZE) / PAGE_SIZE * PAGE_SIZE;
	if (new_sz > g_arena.size)
	{
		printf("*** Error: free(): Arena size corruption. Aborting... ***\n");
		abort();
	}
	g_arena.top_un_sz -= bin_lst_update((uintptr_t)g_arena.top + new_sz);
	if (!brk((void *)((uintptr_t)g_arena.top + new_sz)))
	{
		g_arena.size = new_sz;
		g_arena.top->size = g_arena.size - used - g_arena.top_un_sz;
	}
}

static void			bin_append(t_chk_hdr *bin, t_chk_hdr *freed)
{
	bin->nxt = freed;
	freed->prv = bin;
	freed->nxt = (void *)0;
	if (!chk_is_highest(freed))
	{
		if ((g_arena.top_un_sz += freed->size) + g_arena.top->size > PAGE_CACHE * PAGE_SIZE)
			 arena_dealloc();
	}
}

void				free(void *ptr)
{
	t_chk_hdr		*bin;
	t_chk_hdr		*freed;

	if (!ptr)
		return ;
	pthread_mutex_lock(&g_arena.lock);
	freed = (void *)((uintptr_t)ptr - CHK_HDR_SZ);
	if (!is_ptr_valid(freed))
	{
		bin = g_arena.top;
		while (bin->nxt && bin->nxt != freed)
			bin = bin->nxt;
		if (bin->nxt == freed)
			double_free(ptr);
		else
			bin_append(bin, freed);
	}
	pthread_mutex_unlock(&g_arena.lock);
}
