#include "malloc.h"
#include <string.h>

void	*realloc(void *ptr, size_t size)
{
  void	*new_ptr;

  if (!ptr)
    return malloc(size);
  else if (!size)
    {
      free(ptr);
      return NULL;
    }
  return memcpy(new_ptr, ptr, size);
}
