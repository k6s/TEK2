#include "malloc.h"

void			bin_lst_pop(t_chk_hdr *bin)
{
	if (!bin->prv || bin->prv->nxt != bin
		|| (bin->nxt && bin->nxt->prv != bin))
		verb_abort(bin, "malloc", "corrupted bins linked list");
	bin->prv->nxt = bin->nxt;
	if (bin->nxt)
		bin->nxt->prv = bin->prv;
}

void			*bin_lst_find(t_chk_hdr *free_chk, size_t size)
{
	while (free_chk && free_chk->size < size)
		free_chk = free_chk->nxt;
	if (free_chk)
	{
		bin_lst_pop(free_chk);
		if (!chk_is_highest(free_chk))
			g_arena.top_un_sz -= free_chk->size;
	}
	return (free_chk);
}

size_t			bin_lst_refresh(uintptr_t limit)
{
	t_chk_hdr	*chk;
	size_t		tfree;

	chk = g_arena.top->nxt;
	tfree = 0;
	while (chk)
	{
		if ((uintptr_t)chk + chk->size >= limit)
		{
			bin_lst_pop(chk);
			tfree += chk->size;
		}
		chk = chk->nxt;
	}
	return (tfree);
}
