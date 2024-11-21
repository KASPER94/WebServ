CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_FILES = main Conf Client Server Epoller Log 

SRC_DIR = ./src/

OBJ_DIR = ./obj/

SRC = $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FILES)))

OBJ = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))

NAME = webserv

GREEN = \e[1;32m
YELLOW=\e[0;33m
BLUE=\e[0;34m
NONE = \e[0m

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
