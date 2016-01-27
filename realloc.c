#include "malloc.h"
#include <string.h>

void		*realloc(void *ptr, size_t size)
{
  void 		*new_ptr;
  t_chk_hdr	*chk;

  if (!ptr)
    return malloc(size);
  else if (!size)
    {
      free(ptr);
      return NULL;
    }
  if ((t_chk_hdr)((uintptr_t)ptr - CHK_HDR_SZ)->size == size)
    return (ptr);
  new_ptr = malloc(size);
  memcpy(new_ptr, ptr, size);
  free(ptr);
  return (new_ptr);
}
