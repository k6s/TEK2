#include "malloc.h"
void				verb_abort(t_chk_hdr *chk, const char *pname,
						   const char *msg)
{
	fprintf(stdout, "*** Error: %s(): %s: %p ***\n", pname, msg, chk);
	abort();
}


static void			show_chk(void)
{
	t_chk_hdr		*chk;
	size_t			sz = 0;

	sz += g_arena.top->size;
	chk = (void *)((uintptr_t)g_arena.top + BIN_HDR_SZ);
	while (sz < g_arena.size)
	{
		printf("0x%lx - 0x%lx: 0x%04lx\n", (uintptr_t)chk,
				(uintptr_t)chk + (uintptr_t)chk->size,
				(uintptr_t)chk->size);
		sz += chk->size;
		chk_is_valid(chk, sz);
		chk = (void *)((uintptr_t)chk + chk->size);
	}
}

static void			show_bin(void)
{
	t_chk_hdr		*bin;

	bin = g_arena.top->nxt;
	printf("Free Bins List: %p\n", g_arena.top->nxt);
	while (bin)
	{
		printf("0x%04lx - 0x%04lx: 0x%04lx\n", (uintptr_t)bin,
				(uintptr_t)bin + (uintptr_t)bin->size,
				(uintptr_t)bin->size);
		bin = bin->nxt;
	}
}

void				show_alloc_mem(void)
{

	printf("break : 0x%04lx %p\n", (uintptr_t)sbrk(0), g_arena.top);
	if (g_arena.top)
	{
		show_chk();
		show_bin();
	}
}
