VPATH	=	parse requestHandler responseHandler responseHandler/autoindex
CC		=	c++
CFLAGS	=	-std=c++98 -Wall -Wextra -Werror -MD #-fsanitize=address

NAME	=	webserv

SRC		=	main.cpp \
			autoindex.cpp \
			ConfigFile.cpp parser_utils.cpp Parser.cpp \
			request_utils.cpp RequestParser.cpp \
			ResponseHandler.cpp

OBJ		=	$(addprefix obj/,$(notdir $(SRC:.cpp=.o)))
INC		=	-I parse -I requestHandler -I responseHandler

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