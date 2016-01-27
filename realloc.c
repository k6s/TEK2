#include "malloc.h"
#include <string.h>

void		*realloc(void *ptr, size_t size)
{
  void 		*new_ptr;
  t_chk_hdr	*chk;
  size_t	old_size;
  
  if (!ptr)
    return malloc(size);
  else if (!size)
    {
      free(ptr);
      return NULL;
    }
  if ((old_size = ((t_chk_hdr *)((uintptr_t)ptr - CHK_HDR_SZ))->size) == size)
    ;
  if ((new_ptr = malloc(size)))
	 memcpy(new_ptr, ptr, old_size < size ? old_size : size);
  free(ptr);
  printf("got %p\n", new_ptr);
  return (new_ptr);
}
