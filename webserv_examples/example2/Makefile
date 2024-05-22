<<<<<<< HEAD
<<<<<<< HEAD
.PHONY:	all clean fclean re

SRC_DIR = srcs/
SRC_MSG_DIR = $(SRC_DIR)msg_format/
OBJ_DIR = obj/
HEADER_DIR = headers/
HEADER_MSG_DIR = $(HEADER_DIR)msg_format/

SRC = config_blocks.cpp \
		parser.cpp \
		server.cpp \
		location.cpp \
		socket.cpp \
		request.cpp \
		response.cpp \
		Cgi.cpp \
		webserver.cpp \
		webserv.cpp
SRC_MSG = Body.cpp RequestLine.cpp StatusLine.cpp
SRC_MAIN = $(SRC_DIR)main.cpp

OBJ = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC))
OBJ_MSG = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC_MSG))
OBJ_MAIN = $(OBJ_DIR)main.o

HEADER = $(patsubst %.cpp, $(HEADER_DIR)%.hpp, $(SRC))
CXX	= clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -fstandalone-debug -fsanitize=address

NAME = webserver

all: $(NAME)

$(NAME):		$(OBJ) $(OBJ_MSG) $(OBJ_MAIN) $(HEADER) Makefile
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) $(OBJ_MSG) $(OBJ_MAIN)
	@echo "$@ compiled successfully."

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp $(HEADER_DIR)%.hpp
	@echo -n "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -I$(HEADER_DIR)
	@echo " success."
	@mkdir -vp $(@D)
	@mv -v $(@F) $(@D)

$(OBJ_DIR)%.o:	$(SRC_MSG_DIR)%.cpp $(HEADER_MSG_DIR)%.hpp
	@echo -n "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -I$(HEADER_DIR)
	@echo " success."
	@mkdir -vp $(@D)
	@mv -v $(@F) $(@D)

$(OBJ_MAIN):	$(SRC_MAIN)
	@echo -n "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $(SRC_MAIN) -I$(HEADER_DIR)
	@echo " success."
	@mkdir -vp $(OBJ_DIR)
	@mv -v $(@F) $(@D)

clean:
	@rm -rfv $(NAME)

fclean:		clean
	@rm -rfv $(OBJ_DIR)

re:	fclean all
=======
=======
>>>>>>> 4fda795 (cleanup)
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ael-khni <ael-khni@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/24 17:30:02 by ael-khni          #+#    #+#              #
#    Updated: 2023/01/28 21:53:41 by ael-khni         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= c++ -g
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
RM			= rm -rf

FSANITIZE	= # -fsanitize=address

CXXFLAGS    += ${FSANITIZE}

OBJDIR		= build

INCLUDES	:= $(shell find . -name '*.hpp' -type f -exec dirname {} \; | uniq | sed 's/^/-I /' | grep -v Playground)
SOURCES     := $(shell find . -name '*.cpp' -print -type f | grep -v Playground)
HEADERS		:= $(shell find . -name '*.hpp' -print -type f | grep -v Playground)

OBJ			= $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# Colors:
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
PURPLE		=	\e[35;5;141m
BLUE		=	\e[34;5;75m
WHITE		=	\e[97;5;255m
RED			=	\e[91;5;196m
RESET		=	\e[0m

# Symbols:
SUCCESS		=	\e[92;5;118m[ SUCCESS ]\e[0m
ERROR		=	\e[91;5;196m[ ERROR ]\e[0m
WARNING		=	\e[93;5;226m[ WARNING ]\e[0m
INFO		=	${PURPLE}[ INFO ]${RESET}

# Styles:
CURSIVE		=	\e[33;3m
BOLD		=	\e[33;1m
UNDERLINE	=	\e[33;4m
CROSS		=	\e[33;9m


#Debug
ifeq ($(DEBUG), 1)
   OPTS = -g
endif

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ) $(HEADERS)
	@$(CC) $(INCLUDES) $(CXXFLAGS) $(OBJ) $(OPTS) -o $(NAME)
	@printf "\n\n$(SUCCESS) $(GREEN)- Executable ready: $(BOLD)$(UNDERLINE)${RED}$(NAME)$(RESET)\n\n"

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@printf "${CURSIVE}${GRAY}	- Making object file: ${BOLD}${GREEN}%-20s ${RESET}\r" $(notdir $@)
	@$(CC) $(INCLUDES) $(CXXFLAGS) $(OPTS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR) $(OBJ)
	@printf "\n${INFO} - Object files ${BOLD}${UNDERLINE}${WHITE}removed${RESET}.\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "${INFO} - Executable [${BOLD}${UNDERLINE}${RED}${NAME}${RESET}] removed.$(RESET)\n\n"

re: fclean all
<<<<<<< HEAD
>>>>>>> 8b3d215 (add examples)
=======
=======
.PHONY:	all clean fclean re

SRC_DIR = srcs/
SRC_MSG_DIR = $(SRC_DIR)msg_format/
OBJ_DIR = obj/
HEADER_DIR = headers/
HEADER_MSG_DIR = $(HEADER_DIR)msg_format/

SRC = config_blocks.cpp \
		parser.cpp \
		server.cpp \
		location.cpp \
		socket.cpp \
		request.cpp \
		response.cpp \
		Cgi.cpp \
		webserver.cpp \
		webserv.cpp
SRC_MSG = Body.cpp RequestLine.cpp StatusLine.cpp
SRC_MAIN = $(SRC_DIR)main.cpp

OBJ = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC))
OBJ_MSG = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC_MSG))
OBJ_MAIN = $(OBJ_DIR)main.o

HEADER = $(patsubst %.cpp, $(HEADER_DIR)%.hpp, $(SRC))
CXX	= clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -fstandalone-debug -fsanitize=address

NAME = webserver

all: $(NAME)

$(NAME):		$(OBJ) $(OBJ_MSG) $(OBJ_MAIN) $(HEADER) Makefile
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) $(OBJ_MSG) $(OBJ_MAIN)
	@echo "$@ compiled successfully."

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp $(HEADER_DIR)%.hpp
	@echo -n "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -I$(HEADER_DIR)
	@echo " success."
	@mkdir -vp $(@D)
	@mv -v $(@F) $(@D)

$(OBJ_DIR)%.o:	$(SRC_MSG_DIR)%.cpp $(HEADER_MSG_DIR)%.hpp
	@echo -n "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -I$(HEADER_DIR)
	@echo " success."
	@mkdir -vp $(@D)
	@mv -v $(@F) $(@D)

$(OBJ_MAIN):	$(SRC_MAIN)
	@echo -n "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $(SRC_MAIN) -I$(HEADER_DIR)
	@echo " success."
	@mkdir -vp $(OBJ_DIR)
	@mv -v $(@F) $(@D)

clean:
	@rm -rfv $(NAME)

fclean:		clean
	@rm -rfv $(OBJ_DIR)

re:	fclean all
>>>>>>> e4d723c (cleanup)
>>>>>>> 4fda795 (cleanup)
