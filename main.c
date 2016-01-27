#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>

int				main(void)
{
	char		*s[1024];


	s[0] = malloc(0x238);
	memset(s[0], 'a', 0x238);
	s[1] = malloc(0x78);
	memset(s[1], 'a', 0x78);
	free(s[1]);
	free(s[0]);
	s[0] = malloc(0x5);
	free(s[0]);
	s[0] = malloc(0x238);
	s[1] = malloc(0x640);
	s[2] = malloc(0x400);
	s[3] = malloc(0x800);
	free(s[2]);
	free(s[0]);
	s[4] = malloc(0x5);
	s[5] = malloc(0x2d);
	s[6] = malloc(0x2d);
	s[7] = malloc(0x48);
	s[8] = malloc(0x28);
/*	s[9] = malloc(0x78);
	s[10] = malloc(0xa8);
	s[11] = malloc(0x68);
	s[12] = malloc(0x50);
	s[13] = malloc(0xc0);
	s[14] = malloc(0x6); */
	for (int i = 0; i < 14; i++)
		free(s[i]);
	s[0] = malloc(1457664);
	printf("got %p\n", s[0]);
	free(s[0]);
	for (int i = 0; i < 1024; i++)
	{
		s[i] = malloc(i);
		if (i)
			show_alloc_mem();
		memset(s[i], 'a', i);
		free(s[i]);
	}
	for (int i = 1; i < 512; i++)
	{
		free(s[i]);
	} 
	for (int i = 512; i < 1024; i++)
	{
		s[i] = malloc(i);
	}
	for (int i = 512; i < 1024; i++)
	  	free(s[i]);
	return (0);
}
