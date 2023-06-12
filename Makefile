VPATH	=	server/cgi server/colored_output server/config_parser server/core server/request server/response server/response/autoindex
CC		=	c++
CFLAGS	=	-std=c++98 -Wall -Wextra -MD -Werror

NAME	=	webserv

SRC		=	main.cpp \
			CGIHandler.cpp CGIUtils.cpp \
			color.cpp \
			ConfigFile.cpp ParserUtils.cpp Parser.cpp \
			Core.cpp Server.cpp Socket.cpp \
			RequestParserUtils.cpp RequestParser.cpp \
			autoindex.cpp \
			responseUtils.cpp ResponseHandler.cpp

OBJ_DIR =   server/obj
OBJ     =   $(addprefix $(OBJ_DIR)/,$(notdir $(SRC:.cpp=.o)))
INC		=	-I server/cgi -I server/colored_output -I server/config_parser -I server/core -I server/request -I server/response

.SILENT:

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ 
	@echo "webserv is ready"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(INC) $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -f $(NAME)

re : clean all

.PHONY : all clean fclean re

# Run tests ===========================
check: ./$(NAME)
	./$(NAME)
# Run tests ===========================