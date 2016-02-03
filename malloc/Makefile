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

CFLAGS	:= -fpic -Wall -Wextra -Ofast

LDFLAGS := -shared -lpthread

NAME	= libmy_malloc.so

SRCS	= malloc.c \
		  free.c \
		  show_alloc_mem.c \
		  realloc.c \
		  calloc.c \
		  arena.c \
		  chk.c \
		  bin_lst.c

OBJS	= $(addprefix obj/, $(notdir $(SRCS:.c=.o)))

all:	obj/ $(NAME)

obj/:
	mkdir $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $< -c -I include/

clean:
	rm -rf obj/

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean re fclean all
