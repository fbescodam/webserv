# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: lde-la-h <lde-la-h@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2022/05/13 11:36:28 by lde-la-h      #+#    #+#                  #
#    Updated: 2022/06/17 04:11:40 by pvan-dij      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

#//= Colors =//#
BOLD	:= \033[1m
BLACK	:= \033[30;1m
RED		:= \033[31;1m
GREEN	:= \033[32;1m
YELLOW	:= \033[33;1m
BLUE	:= \033[34;1m
MAGENTA	:= \033[35;1m
CYAN	:= \033[36;1m
WHITE	:= \033[37;1m
RESET	:= \033[0m

#//= Files =//#
CC		:=	clang++
NAME	:=	webserv
# TODO: Pjotr pls explain lol
HEADERS :=	$(addprefix -I , $(shell find ./src -iname '*.hpp' | sed -E "s|/[^/]+$$||" | uniq)) -I include
# Project requires you do it in 98, frankly we don't care (We asked)
CFLAGS	:= -std=c++17 -Wextra -Wall -Werror -Wno-unreachable-code -Wno-char-subscripts -Wno-unused-variable -Wno-unused-parameter
ifdef DEBUG
	CFLAGS	+=	-g3
else
	CFLAGS	+= -D NDEBUG
endif
SRCS	:=	$(shell find ./src -iname "*.cpp")
OBJS	:=	${SRCS:.cpp=.o}

#//= Recipes =//#
all: # Multi threading badness because C++ is slow
	@$(MAKE) $(NAME) -j4

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
