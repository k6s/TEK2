#include "malloc.h"
#include <string.h>

void		*realloc(void *ptr, size_t size)
{
  void 		*new_ptr;
  size_t	old_size;
  
  if (!ptr)
    return malloc(size);
  else if (!size)
    {
      free(ptr);
      return NULL;
    }
  old_size = ((t_chk_hdr *)((uintptr_t)ptr - BIN_HDR_SZ))->size;
  if (old_size >= calc_chk_size(old_size))
    return (ptr);
  if ((new_ptr = malloc(size)))
	 memcpy(new_ptr, ptr, old_size < size ? old_size : size);
  free(ptr);
  return (new_ptr);
}
