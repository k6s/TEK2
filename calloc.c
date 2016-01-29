#include "malloc.h"

void				*calloc(size_t nmemb, size_t size)
{
	unsigned char	*new;

	if (!nmemb || !size)
		return ((void *)0);
	if (new = malloc(nmemb * size))
	memset(new, 0, size * nmemb);
	return (new);
}
