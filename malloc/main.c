#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>

int				main(void)
{
	char		**s[1024];


	s[0] = malloc(0x38);
	printf("sbrk(0) %p\n", sbrk(0));
	s[0] = malloc(0x38);
	printf("sbrk(0) %p\n", sbrk(0));
	free(s[0]);
	s[0] = malloc(0x60000);
	printf("sbrk(0) %p\n", sbrk(0));
	free(s[0]);
	printf("sbrk(0) %p\n", sbrk(0));
/*	s[0] = malloc(1457664);
	printf("got %p\n", s[0]);
	free(s[0]); 
	srand(2600);
	while (1)
	{
		s[0] = malloc(random() % (640));
		show_alloc_mem();
		if (random() % 2)
		{
			free(s[0]);
			show_alloc_mem();
		}
	} 

	for (int i = 0; i < 1000000; i++)
	{
		s[0] = malloc(i);
		free(s[0]);
	} */
/*	for (int i = 0; i < 512; i++)
	{
//		free(s[i]);
	}
	for (int i = 512; i < 1024; i++)
	{
		s[i] = malloc(i);
	}
	for (int i = 512; i < 1024; i++)
	  	free(s[i]); 
	s[0] = getenv("_");
	if (s[0])
		printf("%s\n", s[0]); */
	return (0);
}
