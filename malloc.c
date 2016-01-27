#include "malloc.h"
#include <stdio.h>

t_heap_hdr	g_arena;

void		*find_free_chk(t_chk_hdr *free_chk, size_t size)
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
//		printf("from free bins list\n");
	}
	return (free_chk);
}

void			*heap_new_page(size_t size)
{
	t_chk_hdr	*chk;
	t_heap_hdr	new_arena;

	size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
	if ((chk = sbrk(size)) == (void *)-1)
		return (NULL);
	if ((new_arena.top = sbrk(0)) == (void *)-1)
		return (NULL);
	new_arena.top = (void *)((uintptr_t)new_arena.top - CHK_HDR_SZ);
	if (!g_arena.top)
		new_arena.size = 0;
	else
		new_arena.size = g_arena.size;
	new_arena.size += size;
	new_arena.top->size = size;
	new_arena.top->nxt = (void *)0;
	new_arena.top->prv = (void *)0;
	if (g_arena.top)
	{
		new_arena.top->size += g_arena.top->size;
		if ((new_arena.top->nxt = g_arena.top->nxt))
			new_arena.top->nxt->prv = new_arena.top;
		new_arena.top->prv = NULL;
	}
	memcpy(&g_arena, &new_arena, HEAP_HDR_SZ);
	return (new_arena.top);
}

void			*wild_split(size_t size)
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
	if (g_arena.top && (chk = find_free_chk(g_arena.top->nxt, size)))
		;
	else if (g_arena.top && (chk = wild_split(size)))
		;
	else if ((chk = heap_new_page(size)))
		chk = wild_split(size);
	assert(!((uintptr_t)chk % ALIGN_SIZE));
//	printf("malloc %x %p - %x left\n", size, (uintptr_t)chk + CHK_HDR_SZ, g_arena.top->size);
//	if ((uintptr_t)chk + CHK_HDR_SZ ==  0x422c608)
//	show_alloc_mem();
	return ((void *)((uintptr_t)chk + CHK_HDR_SZ));
}
