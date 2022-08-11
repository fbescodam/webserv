# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: lde-la-h <lde-la-h@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2022/05/13 11:36:28 by lde-la-h      #+#    #+#                  #
#    Updated: 2022/08/11 19:55:43 by fbes          ########   odam.nl          #
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
HEADERS :=	$(addprefix -I , $(shell find ./src -iname '*.hpp' | sed -E "s|/[^/]+$$||" | uniq)) -I include
# Project requires you do it in 98, frankly we don't care (We asked)
CFLAGS	:= -std=c++17 -Wextra -Wall -Werror -Wunreachable-code -Wno-char-subscripts -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare
ifdef DEBUG
	CFLAGS	+=	-g3
else ifdef FSAN
	CFLAGS	+=	-fsanitize=address -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address"
else
	CFLAGS	+= -D NDEBUG -Ofast
endif
SRCS	:=	$(shell find ./src -iname "*.cpp")
OBJS	:=	${SRCS:.cpp=.o}

#//= Recipes =//#
all: # Multi threading badness because C++ is slow
	@$(MAKE) $(NAME) -j4

%.o: %.cpp
	@$(CC) $(CFLAGS) -o $@ -c $< $(HEADERS) && printf "$(GREEN)$(BOLD)\rCompiling: $(notdir $<)\r\e[35C[OK]\n$(RESET)"

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)$(BOLD)Done\n$(RESET)"

clean:
	@echo "$(RED)Cleaning$(RESET)"
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re:	fclean all

er: re # For Freek who cannot type on Sundays

.PHONY: all clean fclean re er
