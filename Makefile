VPATH	=	cgi parse request response response/autoindex
CC		=	c++
CFLAGS	=	-std=c++98 -Wall -Wextra -Werror -MD -fsanitize=address

NAME	=	webserv

SRC		=	main.cpp \
			CGIHandler.cpp cgi_utils.cpp \
			ConfigFile.cpp parser_utils.cpp Parser.cpp \
			request_utils.cpp RequestParser.cpp \
			autoindex.cpp \
			response_utils.cpp ResponseHandler.cpp

OBJ		=	$(addprefix obj/,$(notdir $(SRC:.cpp=.o)))
INC		=	-I cgi -I parse -I request -I response

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