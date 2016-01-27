#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>

int				main(void)
{
	char		*s[1024];


/*	s[0] = malloc(0x238);
	memset(s[0], 'a', 0x238);
	s[1] = malloc(0x78);
	memset(s[1], 'a', 0x78);
	free(s[1]);
	free(s[0]);
	s[0] = malloc(0x5);
	memset(s[0], 'a', 5);
	free(s[0]);
	s[0] = malloc(0x238);
	memset(s[0], 'a', 0x238);
	s[1] = malloc(0x640);
	memset(s[1], 'a', 0x640);
	s[2] = malloc(0x400);
	memset(s[2], 'a', 0x400);
	s[3] = malloc(0x800);
	memset(s[3], 'a', 0x800);
	free(s[2]);
	free(s[0]);
	s[4] = malloc(0x5);
	memset(s[4], 'a', 0x5);
	s[5] = malloc(0x2d);
	memset(s[5], 'a', 0x2d);
	s[6] = malloc(0x2d);
	memset(s[6], 'a', 0x2d);
	s[7] = malloc(0x48);
	memset(s[7], 'a', 0x48);
	s[8] = malloc(0x28);
	memset(s[7], 'a', 0x28);
	s[9] = malloc(0x78);
	s[10] = malloc(0xa8);
	s[11] = malloc(0x68);
	s[12] = malloc(0x50);
	s[13] = malloc(0xc0);
	s[14] = malloc(0x6);
	for (int i = 0; i < 14; i++)
		free(s[i]);
	s[0] = malloc(1457664);
	printf("got %p\n", s[0]);
	free(s[0]); */
/*	for (int i = 0; i < 20; i++)
	{
		s[i] = malloc(i);
		if (i)
			show_alloc_mem();
		memset(s[i], 'a', i);
	}
	for (int i = 1; i < 10; i++)
	{
		free(s[i]);
	} 
	for (int i = 10; i < 20; i++)
	{
		s[i] = malloc(i);
	}
	for (int i = 10; i < 20; i++)
	  	free(s[i]);
	return (0);
	s[0] =  NULL;
	s[1] =  NULL; */
	/*
	for (int i = 0; i < 1024; i++)
	{
		s[0] = realloc(s[0], 6);
		s[1] = realloc(s[1], 6);
		memset(s[1], 'a', 6);
		memcpy(s[0], s[1], 6);
	} */
	s[0] = malloc(2);
	show_alloc_mem();
	s[1] = malloc(5);
	show_alloc_mem();
	free(s[0]);
	show_alloc_mem();
	free(s[1]);
	show_alloc_mem();
	s[0] = malloc(2);
	show_alloc_mem();
	s[1] = malloc(5);
	s[2] = malloc(20);
	s[3] = malloc(30);
	show_alloc_mem();
	free(s[0]);
	free(s[2]);
	free(s[3]);
	free(s[1]);
	s[1] = malloc(5);
	show_alloc_mem();
	free(s[1]);
	return (0);
}
