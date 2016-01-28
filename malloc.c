#include "malloc.h"

t_heap_hdr		g_arena;

static void		*find_free_chk(t_chk_hdr *free_chk, size_t size)
{
	size = ALIGN(size + CHK_HDR_SZ);
	while (free_chk && free_chk->size < size)
		free_chk = free_chk->nxt;
	if (free_chk)
	{
		if (free_chk->prv)
			free_chk->prv->nxt = free_chk->nxt;
		if (free_chk->nxt)
			free_chk->nxt->prv = free_chk->prv;
		free_chk->nxt = NULL;
		free_chk->prv = NULL;
	}
	return (free_chk);
}

static void				heap_set_hdr(t_heap_hdr *new_arena, size_t size)
{
	new_arena->size = size;
	new_arena->top->size = size;
	new_arena->top->nxt = (void *)0;
	new_arena->top->prv = (void *)0;
	if (g_arena.top)
	{
		new_arena->size += g_arena.size;
		new_arena->top->size += g_arena.top->size;
		if ((new_arena->top->nxt = g_arena.top->nxt))
			new_arena->top->nxt->prv = new_arena->top;
		new_arena->top->prv = NULL;
	}
	memcpy(&new_arena->lock, &g_arena.lock, sizeof(g_arena.lock));
	new_arena->ilock = g_arena.ilock;
	memcpy(&g_arena, new_arena, HEAP_HDR_SZ);
}

static void		*heap_new_page(size_t size)
{
	t_chk_hdr	*chk;
	t_heap_hdr	new_arena;

	size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
	if (!g_arena.top && size < PAGE_CACHE * PAGE_SIZE)
		size = PAGE_CACHE * PAGE_SIZE;
	if (size < 1 || size > INTPTR_MAX || (chk = sbrk(size)) == (void *)-1)
		return (NULL);
	if ((new_arena.top = sbrk(0)) == (void *)-1)
		return (NULL);
	new_arena.top = (void *)((uintptr_t)new_arena.top - CHK_HDR_SZ);
	heap_set_hdr(&new_arena, size);
	return (new_arena.top);
}

static void		*wild_split(size_t size)
{
	t_chk_hdr	*chk;

	size = ALIGN(size + CHK_HDR_SZ);
	if (size + CHK_HDR_SZ >= g_arena.top->size)
		return (NULL);
	chk = (t_chk_hdr *)((uintptr_t)g_arena.top + CHK_HDR_SZ
						- g_arena.top->size);
	g_arena.top->size -= size;
	chk->size = size;
	chk->prv = (void *)0;
	chk->nxt = (void *)0;
	return (chk);
}

#include <assert.h>

void					*malloc(size_t size)
{
	t_chk_hdr			*chk = NULL;

	if (!size)
		return (NULL);
	if (!g_arena.ilock && (g_arena.ilock = 1)
	   	&& pthread_mutex_init(&g_arena.lock, NULL))
			return (NULL);
	pthread_mutex_lock(&g_arena.lock);
	if (g_arena.top && (chk = find_free_chk(g_arena.top->nxt, size)))
		;
	else if (g_arena.top && (chk = wild_split(size)))
		;
	else if ((chk = heap_new_page(size)))
		chk = wild_split(size);
	assert(!((uintptr_t)chk % ALIGN_SIZE));
	pthread_mutex_unlock(&g_arena.lock);
	return (chk ? (void *)((uintptr_t)chk + CHK_HDR_SZ) : NULL);
}
