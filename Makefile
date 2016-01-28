##
## Makefile for malloc in /home/faure-_l/TEK2/PSU/PSU_2015_malloc
## 
## Made by Loïk Fauré-Berlinguette
## Login   <faure-_l@epitech.net>
## 
## Started on  Tue Jan 26 00:39:58 2016 Loïk Fauré-Berlinguette
## Last update Tue Jan 26 01:12:57 2016 Loïk Fauré-Berlinguette
##

CC	= gcc

RM	= rm -f

CFLAGS	= -fpic -Wall -Wextra

LDFLAGS = -shared

NAME	= libmy_malloc.so

SRCS	= malloc.c free.c show_alloc_mem.c realloc.c calloc.c

OBJS	= $(SRCS:.c=.o)

all:	$(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) -shared -lpthread

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $< -c -ggdb

clean:
	$(RM) $(OBJS)

fclean:
	$(RM) $(NAME)

re: fclean all
