#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>

int				main(void)
{
	char		**s[1024];


/*	s[0] = malloc(1457664);
	printf("got %p\n", s[0]);
	free(s[0]); */ 
	for (int i = 0; i < 1024; i++)
	{
		s[i] = malloc(i);
		printf("malloc(%u): %p\n", i, s[i]);
		memset(s[i], 'a', i);
		free(s[i]);
	}
	for (int i = 0; i < 512; i++)
	{
//		free(s[i]);
	}
	for (int i = 512; i < 1024; i++)
	{
		s[i] = malloc(i);
	}
	for (int i = 512; i < 1024; i++)
	 	free(s[i]);
	return (0);
}
