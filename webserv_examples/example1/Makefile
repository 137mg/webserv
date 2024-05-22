<<<<<<< HEAD
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trossel <trossel@42lausanne.ch>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/07 17:50:59 by trossel           #+#    #+#              #
#    Updated: 2022/11/04 11:37:25 by trossel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TARGET		= webserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic
CPPFLAGS	= -Iincludes
SRC_DIR		= src
OBJS_DIR	= obj
OBJS = 		$(patsubst %.cpp, $(OBJS_DIR)/%.o, $(SRCS))
SRCS		= main.cpp \
			  Config.cpp \
			  RedirResponse.cpp \
			  ErrorResponse.cpp \
			  ValidResponse.cpp \
			  GetResponse.cpp \
			  PostResponse.cpp \
			  DeleteResponse.cpp \
			  Response.cpp \
			  Looper.cpp \
			  Server.cpp \
			  Location.cpp \
			  DirectoryListing.cpp \
			  Utils.cpp \
			  ConfigParser.cpp \
			  Json.cpp \
			  RequestParser.cpp \
			  CommonGatewayInterface.cpp \
			  Request.cpp

all: $(TARGET)

UNAME= $(shell uname -s)
ifeq ($(UNAME), Darwin)
98: CXX = g++-11
endif
98: CXXFLAGS += -pedantic -std=c++98
98: fclean $(TARGET)

asan: CXXFLAGS += -g3 -fsanitize=address -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address
asan: $(TARGET)

debug: CXXFLAGS += -g3
debug: $(TARGET)

valgrind: CXXFLAGS += -g3
valgrind: fclean $(OBJS) $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -rf $(OBJS_DIR)
fclean: clean
	rm -f $(TARGET)

re: fclean all

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(OBJS_DIR):
	@mkdir -p $(dir $(OBJS))

.PHONY: all clean fclean re test 98 debug valgrind
=======
NAME = webserv

SRCS = src/main.cpp src/Utils.cpp src/ServerManager.cpp src/Response.cpp src/Client.cpp src/HttpRequest.cpp \
	   src/ConfigFile.cpp src/ConfigParser.cpp src/ServerConfig.cpp src/Location.cpp src/CgiHandler.cpp \
	   src/Mime.cpp src/Logger.cpp

HEADERS	= inc/Webserv.hpp

OBJS = $(SRCS:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXXFLAGS += -g3 

RM = rm -rf

RESET = "\033[0m"
BLACK = "\033[1m\033[37m"

all:
	@$(MAKE) $(NAME) -j5
$(NAME) : $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo $(BLACK)-webserv compiled 🌐 $(RESET)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: 	fclean all

.PHONY: all clean fclean re
>>>>>>> 8b3d215 (add examples)
