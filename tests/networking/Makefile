SRC_DIR			=	src
OBJ_DIR			=	obj
CC				=	c++
CPPFLAGS		=	-std=c++11

NAME			=	webserv

SRC				=	$(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJ				=	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
INCLUDE 		=	-I incl

.SILENT:

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CPPFLAGS) $(INCLUDE) -o $@ $^ 
	echo "server is ready to run"

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | obj
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

obj :
	mkdir obj

clean :
	rm -rf obj

fclean : clean
	rm -f $(NAME)

re : clean all

.PHONY : all clean fclean re

# Run tests ===========================
check: ./$(NAME)
	./$(NAME)
# Run tests ===========================