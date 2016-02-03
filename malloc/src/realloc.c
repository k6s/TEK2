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
  old_size = ((t_chk_hdr *)((uintptr_t)ptr - CHK_HDR_SZ))->size;
  if (old_size >= to_chk_size(size))
    return (ptr);
  if ((new_ptr = malloc(size)))
	 memcpy(new_ptr, ptr, old_size < size ? old_size : size);
  free(ptr);
  return (new_ptr);
}
