# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/08 11:22:23 by psadeghi          #+#    #+#              #
#    Updated: 2024/05/31 16:07:58 by mgoedkoo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

PROJECT = Webserv
INCLUDE = -I include
NAME = webserv
CC = clang++
CPPFLAGS = -Wall -Werror -Wextra -Wshadow -Wno-shadow -std=c++11
OBJ_DIR	= obj/
SRC_DIR	= src/
UPLOAD_DIR = cgi-bin/uploads
SRC =	src/CGI/CGI.cpp							\
		src/Config/Config.cpp					\
		src/Config/ConfigPrint.cpp				\
		src/Config/ConfigUpdate.cpp				\
		src/Config/ConfigUtils.cpp				\
		src/Server/Server.cpp					\
		src/Server/ServerMethods.cpp			\
		src/Server/ServerRequest.cpp			\
		src/Server/ServerUploads.cpp			\
		src/ServerManager/ConfigFile.cpp		\
		src/ServerManager/selectServer.cpp		\
		src/ServerManager/ServerManager.cpp		\
		src/ServerManager/Client.cpp			\
		src/Logger.cpp							\
		src/main.cpp
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

all: $(NAME)

$(NAME): $(OBJ)
	@echo "---------$(BOLD)Linking...$(RESET)---------"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)$(BOLD)["$(NAME)"]$(RESET)" "ready for execution!"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BOLD)Compiled ✅ $(GREEN)$< $(RESET)"
	@$(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@if [ -d "$(OBJ_DIR)" ]; then \
	echo "$(BOLD)Cleaning $(GREEN)obj folder$(RESET)"; \
	fi
	@rm -rf $(OBJ_DIR)
	@rm -rf $(UPLOAD_DIR)

fclean: clean
	@if [ -f "$(NAME)" ]; then \
	echo "$(BOLD)Cleaning $(GREEN)executable$(RESET)"; \
	fi
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
