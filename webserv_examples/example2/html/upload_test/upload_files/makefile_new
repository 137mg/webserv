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
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) $(OBJ_MSG) $(OBJ_MAIN)

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
