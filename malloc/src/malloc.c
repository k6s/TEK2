#include "malloc.h"

t_arena_hdr		g_arena;

static void						*wild_split(size_t size)
{
	t_chk_hdr	*chk;

	if (size + BIN_HDR_SZ >= g_arena.top->size)
		return (NULL);
	chk = (t_chk_hdr *)((uintptr_t)g_arena.top + g_arena.size + BIN_HDR_SZ
					 - g_arena.top->size);
	g_arena.top->size -= size;
	chk->size = size;
	return (chk);
}

#include <assert.h>

static t_chk_hdr				*alloc_chk(size_t size)
{
	t_chk_hdr					*chk;

	chk = NULL;
	size = to_chk_size(size);
	if (g_arena.top && (chk = bin_lst_find(g_arena.top->nxt, size)))
		;
	else if (g_arena.top && (chk = wild_split(size)))
		;
	else if ((chk = arena_alloc(size)))
		chk = wild_split(size);
	return (chk);
}

void __attribute__((malloc))	*malloc(size_t size)
{
	t_chk_hdr	*chk = NULL;

	if (!size)
		return (NULL);
	if (!g_arena.ilock && (g_arena.ilock = 1)
	   	&& (pthread_mutex_init(&g_arena.lock, NULL)))// || gen_heap_guard())
			return (NULL);
	pthread_mutex_lock(&g_arena.lock);
	chk = alloc_chk(size);
	assert(!((uintptr_t)chk % ALIGN_SIZE));
	pthread_mutex_unlock(&g_arena.lock);
	return (chk ? (void *)((uintptr_t)chk + CHK_HDR_SZ) : NULL);
}
