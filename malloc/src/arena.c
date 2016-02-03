#include "malloc.h"

void			arena_set_hdr(t_arena_hdr *new_arena, size_t size)
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

static size_t	arena_resize(size_t size)
{
	size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
	if (size < PAGE_CACHE * PAGE_SIZE)
		size = PAGE_CACHE * PAGE_SIZE;
	if (size < 1 || size > INTPTR_MAX || sbrk(size) == (void *)-1)
		return (0);
	return (size);
}

void			*arena_alloc(size_t size)
{
	t_arena_hdr	new_arena;

	if (!(size = arena_resize(size)))
		return (NULL);
	if ((new_arena.top = sbrk(0)) == (void *)-1)
		return (NULL);
	new_arena.top = (void *)((uintptr_t)new_arena.top - g_arena.size - size);
	if (g_arena.top && g_arena.top != new_arena.top)
		verb_abort(new_arena.top, "free", "Arena size corruption");
	arena_set_hdr(&new_arena, size);
	return (new_arena.top);
}

void			arena_dealloc(void)
{
	size_t		new_sz;
	size_t		used;

	used = g_arena.size - g_arena.top_un_sz - g_arena.top->size;
	new_sz = (used + PAGE_CACHE * PAGE_SIZE) / PAGE_SIZE * PAGE_SIZE;
	if (new_sz > g_arena.size)
		verb_abort((void *)new_sz, "free", "Arena size corruption");
	g_arena.top_un_sz -= bin_lst_refresh((uintptr_t)g_arena.top + new_sz);
	if (!brk((void *)((uintptr_t)g_arena.top + new_sz)))
	{
		g_arena.size = new_sz;
		g_arena.top->size = g_arena.size - used - g_arena.top_un_sz;
	}
}
