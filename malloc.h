#ifndef MALLOC_H_
# define MALLOC_H_

# include <unistd.h>
# include <stdint.h>
# include <linux/types.h>

# ifndef PAGE_SIZE
#  define PAGE_SIZE			((unsigned)getpagesize())
# endif
# define ALIGN_SIZE			__alignof__(unsigned long long)
# define IS_DISALIGNED(x)	((x) % ALIGN_SIZE)
# define DO_ALIGN(x)		((x) + ALIGN_SIZE - ((x) % ALIGN_SIZE))
# define ALIGN(x)			(IS_DISALIGNED(x) ? DO_ALIGN(x) : (x))

typedef struct s_chk_hdr		t_chk_hdr;
typedef struct s_heap_hdr		t_heap_hdr;

struct			s_heap_hdr
{
	size_t		size;
};

# define HEAP_HDR_SZ		sizeof(t_heap_hdr)

struct			s_chk_hdr
{
	size_t		size;
	t_chk_hdr	*nxt;
	t_chk_hdr	*prv;
};

# define CHK_HDR_SZ			sizeof(t_chk_hdr)
# define CHK_WILD_OFF		(CHK_HDR_SZ + HEAP_HDR_SZ)

#endif
