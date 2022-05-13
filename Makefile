# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: lde-la-h <lde-la-h@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2022/05/13 11:36:28 by lde-la-h      #+#    #+#                  #
#    Updated: 2022/05/13 11:51:01 by lde-la-h      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

#//= Colors =//#
BOLD	= \033[1m
BLACK	= \033[30;1m
RED		= \033[31;1m
GREEN	= \033[32;1m
YELLOW	= \033[33;1m
BLUE	= \033[34;1m
MAGENTA	= \033[35;1m
CYAN	= \033[36;1m
WHITE	= \033[37;1m
RESET	= \033[0m

#//= Files =//#
CC		=	clang++
NAME	=	webserv
HEADERS =	-I ./include
# TODO: Remove last arg before defence!
CFLAGS		:= -std=c++11 -Wextra -Wall -Werror -Wunreachable-code -Wno-char-subscripts -Wno-unused-parameter
ifdef DEBUG
	CFLAGS	+=	-g3
else
	CFLAGS	+=	-Ofast -D NDEBUG
endif
SRCS	=	$(shell find ./src -iname "*.cpp")
OBJS	=	${SRCS:.cpp=.o}

#//= Recipes =//#
all: $(NAME)

%.o: %.cpp
	@$(CC) $(CFLAGS) -o $@ -c $< $(HEADERS) && printf "$(GREEN)$(BOLD)\rCompiling: $(notdir $<)\r\e[35C[OK]\n$(RESET)"

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $(NAME)
	@printf "$(GREEN)$(BOLD)Done\n$(RESET)"

clean:
	@echo "$(RED)Cleaning$(RESET)"
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re