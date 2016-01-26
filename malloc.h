#ifndef MALLOC_H_
# define MALLOC_H_

# include <unistd.h>
# include <stdint.h>
# include <linux/types.h>

# ifndef PAGE_SIZE
#  define PAGE_SIZE			((unsigned)getpagesize())
# endif
# define ALIGN_SIZE			__alignof__(unsigned long)
# define IS_DISALIGNED(x)	((x) % ALIGN_SIZE)
# define DO_ALIGN(x)		((x) + ALIGN_SIZE - ((x) % ALIGN_SIZE))
# define ALIGN(x)			(IS_DISALIGNED(x) ? DO_ALIGN(x) : (x))

typedef struct s_chk_hdr		t_chk_hdr;

struct			s_chk_hdr
{
	size_t		size;
	t_chk_hdr	*nxt;
	t_chk_hdr	*prv;
};

# define CHK_HDR_SZ			sizeof(t_chk_hdr)

#endif
