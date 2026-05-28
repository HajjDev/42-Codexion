# ************************************************************************* #
#                                                                           #
#                                                      :::      ::::::::    #
#  Makefile                                          :+:      :+:    :+:    #
#                                                  +:+ +:+         +:+      #
#  By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+         #
#                                              +#+#+#+#+#+   +#+            #
#  Created: 2026/05/27 09:55:24 by cel-hajj        #+#    #+#               #
#  Updated: 2026/05/27 10:13:22 by cel-hajj        ###   ########.fr        #
#                                                                           #
# ************************************************************************* #

NAME=codexion

INCLUDES=includes
SRC=./src/actions_utils.c \
	./src/actions.c \
	./src/argument_parsing.c \
	./src/coder.c \
	./src/codexion.c \
	./src/initialize.c \
	./src/queue.c \
	./src/utils.c
OBJ=$(SRC:.c=.o)
RM = rm -f
CC=cc
CFLAGS=-Wall -Wextra -Werror -pthread

%.o: %.c
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME) -pthread

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re