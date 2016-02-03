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
		verb_abort(ptr, "free", "Invalid pointer");
		return (-1);
	}
	return (0);
}

static void			bin_append(t_chk_hdr *bin, t_chk_hdr *freed)
{
	bin->nxt = freed;
	freed->prv = bin;
	freed->nxt = (void *)0;
	if (!chk_is_highest(freed))
	{
		if ((g_arena.top_un_sz += freed->size)
			+ g_arena.top->size > PAGE_CACHE * PAGE_SIZE)
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
