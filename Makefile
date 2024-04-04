# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: psadeghi <psadeghi@student.42.fr>            +#+                      #
#                                                    +#+                       #
#    Created: 2024/02/08 11:22:23 by psadeghi      #+#    #+#                  #
#    Updated: 2024/04/04 17:51:22 by juvan-to      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

PROJECT = Webserv
INCLUDE = -I include
NAME = webserv
CC = clang++
CPPFLAGS = -Wall -Werror -Wextra -Wshadow -Wno-shadow -std=c++11
OBJ_DIR	= obj/
SRC_DIR	= src/
SRC = src/main.cpp
SRC_CLIENT = $(SRC_DIR)clientConnection.cpp
SRC_SERVER = $(SRC_DIR)serverConnection.cpp
OBJ_CLIENT = $(OBJ_DIR)clientConnection.o
OBJ_SERVER = $(OBJ_DIR)serverConnection.o
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
RM := rm -rf

BLACK   := \033[30m
RED     := \033[31m
GREEN   := \033[32m
YELLOW  := \033[33m
BLUE 	:= \033[96m
MAGENTA := \033[38;5;206m
CYAN    := \033[36m
WHITE   := \033[37m
RESET   := \033[0m
BOLD    := \033[1m
DIM     := \033[2m
ITALIC  := \033[3m
UNDER   := \033[4m
BLINK   := \033[5m
REVERSE := \033[7m
HIDDEN  := \033[8m
PINK 	:= \033[35m

all: server client

server: $(OBJ_SERVER)
	@echo "Compiled with $(GREEN)$(BOLD)$(CPPFLAGS)$(RESET)"
	@$(CC) $(OBJ_SERVER) $(CPPFLAGS) -o server
	@echo "$(BLUE)$(BOLD)-------------------------------------------"
	@echo "     $(PROJECT) (server) = NOW READY FOR USE!"
	@echo "-------------------------------------------$(RESET)"
	
client: $(OBJ_CLIENT)
	@echo "Compiled with $(GREEN)$(BOLD)$(CPPFLAGS)$(RESET)"
	@$(CC) $(OBJ_CLIENT) $(CPPFLAGS) -o client
	@echo "$(BLUE)$(BOLD)-------------------------------------------"
	@echo "     $(PROJECT) (client) = NOW READY FOR USE!"
	@echo "-------------------------------------------$(RESET)"

	
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BOLD)Compiled ✅ $(END) $(GREEN)$< $(END)"
	@$(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@if [ -d "$(OBJ_DIR)" ]; then \
	echo "$(BOLD)Cleaning $(END)$(GREEN)obj folder$(END)"; \
	fi
	@rm -rf $(OBJ_DIR)

fclean: clean
	@if [ -f "$(NAME)" ]; then \
	echo "$(BOLD)Cleaning $(END)$(GREEN)executable$(END)"; \
	fi
	@rm -rf $(NAME)
	@rm -rf server
	@rm -rf client

re: fclean all

.PHONY: all clean fclean re
