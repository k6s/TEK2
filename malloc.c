#include "malloc.h"
#include <stdio.h>

t_chk_hdr	*chk_last(t_chk_hdr *free_chk)
{
	if (free_chk)
	{
		while (free_chk->nxt)
			free_chk = free_chk->nxt;
	}
	return (free_chk);
}

void		*find_free_chk(t_chk_hdr *free_chk, size_t size)
{
	size = ALIGN(size + CHK_HDR_SZ);
	printf("seraching for a new chk\n");
	free_chk = (t_chk_hdr *)((uintptr_t)sbrk(0) - CHK_HDR_SZ);
	free_chk = free_chk->nxt;
	while (free_chk && free_chk->size < size)
		free_chk = free_chk->nxt;
	if (free_chk)
		printf("found a free chk of size %x\n", free_chk->size);
	return (free_chk);
}

void			*heap_new_page(t_chk_hdr **wilderness, size_t size)
{
	t_chk_hdr	*chk;

	printf("Alloc of a new page\n");
	size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
	if ((chk = sbrk(size)) == (void *)-1 || (chk = sbrk(0)) == (void *)-1)
		return (NULL);
	printf("LIMIT: %p\n", chk);
	chk = sbrk(0) - CHK_HDR_SZ;
	chk->size = size;
	chk->nxt = (void *)0;
	chk->prv = (void *)0;
	if (*wilderness)
	{
		chk->size += (*wilderness)->size;
		chk->nxt = (*wilderness)->nxt;
		chk->prv = NULL;
	}
	*wilderness = chk;
	return (chk);
}

void			*wild_split(t_chk_hdr *wilderness, size_t size)
{
	t_chk_hdr	*chk;
	static size_t	tsize = 0;

	wilderness = (t_chk_hdr *)((uintptr_t)sbrk(0) - CHK_HDR_SZ);
	size = ALIGN(size + CHK_HDR_SZ);
	if (size + CHK_HDR_SZ >= wilderness->size)
		return (NULL);
	printf("SIZE %x %x TOTAL %x\n", size + CHK_HDR_SZ, wilderness->size, tsize);
	chk = (t_chk_hdr *)((size_t)((uintptr_t)wilderness)
						- wilderness->size + CHK_HDR_SZ);
	tsize += size;
	wilderness->size -= size;
	chk->size = size;
	chk->prv = (void *)0;
	chk->nxt = (void *)0;
	return (chk);
}

#include <assert.h>

void					*malloc(size_t size)
{
	static t_chk_hdr	*wilderness = NULL;
	t_chk_hdr			*chk = NULL;

	if (!size)
		return (NULL);
	printf("Mallocing size %x\n", size);
	if (wilderness && (chk = find_free_chk(wilderness->prv, size)))
		;
	else if (wilderness && (chk = wild_split(wilderness, size)))
		;
	else if ((chk = heap_new_page(&wilderness, size)))
		chk = wild_split(chk, size);
	printf("CHK %p %x %x\n", chk, chk->size, wilderness->size);
	printf("CHK %p\n", (uintptr_t)chk + CHK_HDR_SZ);
	assert(!((uintptr_t)chk % ALIGN_SIZE));
	return ((void *)((uintptr_t)chk + CHK_HDR_SZ));
}
