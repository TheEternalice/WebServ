# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/14 15:47:01 by lde-merc          #+#    #+#              #
#    Updated: 2025/08/14 15:58:34 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

vpath %.c srcs 
vpath %.conf conf_file

NAME = cub3D

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -g -I $(INCLUDES)

INCLUDES = includes/
OBJ_DIR = objs/
SRC_DIR = srcs/

INVALID_FILE = empty_file.conf

# Liste des fichiers source
SRC_FILES = main.c File.cpp Server.cpp Client.cpp 
			
# Transforme chaque fichier source en un fichier objet dans $(OBJ_DIR)
OBJS = $(addprefix $(OBJ_DIR), $(SRC_FILES:.c=.o))

all: mkdir_obj $(NAME)

# Compilation des fichiers .c en .o dans le dossier $(OBJ_DIR)
$(OBJ_DIR)%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier objs/ et des sous-répertoires s'ils n'existent pas
mkdir_obj:
	@mkdir -p $(OBJ_DIR)

# Compilation finale
$(NAME): $(OBJS)
	@echo "\033[34mmlx en cours\033[0m"
	@make -C mlx all > /dev/null 2>&1
	@echo "\033[0;32mSUCCESS !\033[0m \033[0;33mmlx\033[0m"
	@echo "\033[34mlibft en cours\033[0m"
	@make -C libft all --silent
	@echo "\033[0;32mSUCCESS !\033[0m \033[0;33mlibft\033[0m"
	@echo "\033[34mCompilation $(NAME) en cours\033[0m"
	@$(CC) $(OBJS) $(FLAGS) -o $(NAME)
	@echo "\033[0;32mSUCCESS !\033[0m \033[0;33m$(NAME)\033[0m"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "\033[0;34mDeleting almost everything !\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;35mDeleting everything !\033[0m"
	@make -C libft fclean --silent
	@make -C mlx clean > /dev/null 2>&1

re: fclean all

val: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) || true

test: all
	@for file in $(INVALID_FILE); do \
		echo ""; \
		echo "Testing $$file..."; \
		./$(NAME) $$file || true; \
	done
	@echo ""; echo "Testing without arguments..."; \
	./$(NAME) || true

test_val: all
	@for file in $(INVALID_FILE); do \
		echo ""; \
		echo "Testing $$file..."; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) $$file || true; \
	done
	@echo ""; echo "Testing without arguments..."; \
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) || true

.PHONY: all clean fclean re