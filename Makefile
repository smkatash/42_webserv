VPATH	=	server/cgi server/colored_output server/config_parser server/core server/request server/response server/response/autoindex
CC		=	c++
CFLAGS	=	-std=c++98 -Wall -Wextra -MD -fsanitize=address -Werror

NAME	=	webserv

SRC		=	main.cpp \
			CGIHandler.cpp CGIUtils.cpp \
			color.cpp \
			ConfigFile.cpp ParserUtils.cpp Parser.cpp \
			Core.cpp Server.cpp Socket.cpp \
			RequestParserUtils.cpp RequestParser.cpp \
			autoindex.cpp \
			responseUtils.cpp ResponseHandler.cpp

OBJ		=	$(addprefix obj/,$(notdir $(SRC:.cpp=.o)))
INC		=	-I server/cgi -I server/colored_output -I server/config_parser -I server/core -I server/request -I server/response

all : $(NAME)

$(NAME) : $(OBJ)
	@$(CC) $(CFLAGS) $(INC) -o $@ $^ 
	@echo "webserv is ready"

obj/%.o : %.cpp | obj
	@$(CC) $(CFLAGS) -c $(INC) $< -o $@

obj :
	@mkdir obj

clean :
	@rm -rf obj

fclean : clean
	@rm -f $(NAME)

re : clean all

.PHONY : all clean fclean re

# Run tests ===========================
check: ./$(NAME)
	./$(NAME)
# Run tests ===========================