#ifndef MALLOC_H_
# define MALLOC_H_

# include <unistd.h>
# include <stdint.h>
# include <linux/types.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stddef.h>

# ifndef PAGE_SIZE
#  define PAGE_SIZE			((unsigned)getpagesize())
# endif

# define PAGE_CACHE			64

# define ALIGN_SIZE			__alignof__(unsigned long long)
# define IS_DISALIGNED(x)	((x) % ALIGN_SIZE)
# define DO_ALIGN(x)		((x) + ALIGN_SIZE - ((x) % ALIGN_SIZE))
# define ALIGN(x)			(IS_DISALIGNED(x) ? DO_ALIGN(x) : (x))

typedef struct s_chk_hdr		t_chk_hdr;
typedef struct s_arena_hdr		t_arena_hdr;

struct				s_arena_hdr
{
	t_chk_hdr		*top;
	size_t			size;
	pthread_mutex_t	lock;
	char			ilock;
	size_t			top_un_sz;
};

# define ARENA_HDR_SZ		sizeof(t_arena_hdr)

extern t_arena_hdr				g_arena;

struct			s_chk_hdr
{
	size_t		size;
	t_chk_hdr	*nxt;
	t_chk_hdr	*prv;
};

# define CHK_HDR_SZ			((uintptr_t)offsetof(t_chk_hdr, nxt))
# define BIN_HDR_SZ			sizeof(t_chk_hdr)

size_t	calc_chk_size(size_t size);
int		chk_is_highest(t_chk_hdr *chk);

void	show_alloc_mem(void);
void	*malloc(size_t size);
void	free(void *ptr);

#endif
