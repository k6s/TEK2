#include "malloc.h"

t_arena_hdr		g_arena;

size_t			calc_chk_size(size_t size)
{
	size = ALIGN(size + CHK_HDR_SZ);
	if (size < BIN_HDR_SZ)
		size = ALIGN(BIN_HDR_SZ);
	return (size);
}

int			chk_is_highest(t_chk_hdr *chk)
{
	if ((uintptr_t)chk + chk->size
		== (uintptr_t)g_arena.top + g_arena.size + BIN_HDR_SZ
		- g_arena.top->size)
		return (0);
	return (-1);
}

static void		*find_free_chk(t_chk_hdr *free_chk, size_t size)
{
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
		if (!chk_is_highest(free_chk))
			g_arena.top_un_sz -= free_chk->size;
	}
	return (free_chk);
}

static void		arena_set_hdr(t_arena_hdr *new_arena, size_t size)
{
	if (!g_arena.top)
	{
		g_arena.size = 0;
		g_arena.top_un_sz = 0;
		g_arena.top = new_arena->top;
		memset(g_arena.top, 0, sizeof(*g_arena.top));
	}
	g_arena.top->size += size;
	g_arena.size += size;
}

static void		*arena_new_page(size_t size)
{
	t_chk_hdr	*chk;
	t_arena_hdr	new_arena;

	size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
	if (size < PAGE_CACHE * PAGE_SIZE)
		size = PAGE_CACHE * PAGE_SIZE;
	if (size < 1 || size > INTPTR_MAX || (chk = sbrk(size)) == (void *)-1)
		return (NULL);
	if ((new_arena.top = sbrk(0)) == (void *)-1)
		return (NULL);
	new_arena.top = (void *)((uintptr_t)new_arena.top - g_arena.size - size);
	if (g_arena.top && g_arena.top != new_arena.top)
	{
		printf("*** Error: free(): Arena size corruption. Aborting... ***\n");
		abort();
	}
	arena_set_hdr(&new_arena, size);
	return (new_arena.top);
}

static void		*wild_split(size_t size)
{
	t_chk_hdr	*chk;

	if (size + BIN_HDR_SZ >= g_arena.top->size)
		return (NULL);
	chk = (t_chk_hdr *)((uintptr_t)g_arena.top + g_arena.size + BIN_HDR_SZ
					 - g_arena.top->size);
	g_arena.top->size -= size;
	chk->size = size;
	chk->prv = (void *)0;
	chk->nxt = (void *)0;
	return (chk);
}

#include <assert.h>

void			*malloc(size_t size)
{
	t_chk_hdr	*chk = NULL;

	if (!size)
		return (NULL);
	size = calc_chk_size(size);
	if (!g_arena.ilock && (g_arena.ilock = 1)
	   	&& pthread_mutex_init(&g_arena.lock, NULL))
			return (NULL);
	pthread_mutex_lock(&g_arena.lock);
	if (g_arena.top && (chk = find_free_chk(g_arena.top->nxt, size)))
		;
	else if (g_arena.top && (chk = wild_split(size)))
		;
	else if ((chk = arena_new_page(size)))
		chk = wild_split(size);
	assert(!((uintptr_t)chk % ALIGN_SIZE));
	pthread_mutex_unlock(&g_arena.lock);
	return (chk ? (void *)((uintptr_t)chk + CHK_HDR_SZ) : NULL);
}
