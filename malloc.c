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
	while (free_chk && (free_chk->size + CHK_HDR_SZ) < size)
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
	chk = sbrk(0) - CHK_HDR_SZ;
	printf("LIMIT: %p\n", chk);
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

	wilderness = sbrk(0) - CHK_HDR_SZ;
	size = ALIGN(size + CHK_HDR_SZ);
	if (size + CHK_HDR_SZ >= wilderness->size)
		return (NULL);
	printf("SIZE %x %x\n", size, wilderness->size);
	chk = (t_chk_hdr *)((size_t)((size_t)wilderness + size + CHK_HDR_SZ)
					   	- wilderness->size);
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
	printf("Mallocing size %u\n", size);
	if (wilderness && (chk = find_free_chk(wilderness->prv, size)))
		;
	else if (wilderness && (chk = wild_split(wilderness, size)))
		;
	else if ((chk = heap_new_page(&wilderness, size)))
		chk = wild_split(chk, size);
	printf("CHK %p %lld\n", chk, chk->size);
	printf("CHK %p\n", (uintptr_t)chk + CHK_HDR_SZ);
	assert(!((uintptr_t)chk % ALIGN_SIZE));
	return ((uintptr_t)chk + CHK_HDR_SZ);
}
