##
## Makefile for malloc in /home/faure-_l/TEK2/PSU/PSU_2015_malloc
## 
## Made by Loïk Fauré-Berlinguette
## Login   <faure-_l@epitech.net>
## 
## Started on  Tue Jan 26 00:39:58 2016 Loïk Fauré-Berlinguette
## Last update Wed Jan 27 11:17:48 2016 Loïk Fauré-Berlinguette
##

CC	= gcc

RM	= rm -f

CFLAGS	= -fpic -Wall -Wextra

LDFLAGS = -shared

NAME	= libmy_malloc.so

SRCS	= malloc.c free.c

OBJS	= $(SRCS:.c=.o)

all:	$(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $< -c -ggdb

clean:
	$(RM) $(OBJS)

fclean:
	$(RM) $(NAME)

re: fclean all
