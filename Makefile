# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gebz <gebz@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/14 15:47:01 by lde-merc          #+#    #+#              #
#    Updated: 2025/08/25 16:36:17 by gebz             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

vpath %.cpp srcs 
vpath %.conf conf_file

NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -g -I -std=c++98 $(INCLUDES)

INCLUDES = includes/
OBJ_DIR = objs/
SRC_DIR = srcs

INVALID_FILE = empty_file.conf not_good_name.con

# Liste des fichiers source
SRC_FILES = main.cpp\
			Utils.cpp\
			parsing/Config.cpp\
			parsing/File.cpp\
			parsing/LocationConfig.cpp\
			exec/Server.cpp\
			exec/Client.cpp\
			exec/Request.cpp
			
# Transforme chaque fichier source en un fichier objet dans $(OBJ_DIR)
OBJS = $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

all: mkdir_obj $(NAME)

# Compilation des fichiers .cpp en .o dans le dossier $(OBJ_DIR)
$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) -c $< -o $@

# Création du dossier objs/ et des sous-répertoires s'ils n'existent pas
mkdir_obj:
	@mkdir -p $(OBJ_DIR)

# Compilation finale
$(NAME): $(OBJS)
	@echo "\033[34mCompilation $(NAME) en cours\033[0m"
	@$(CXX) $(OBJS) $(FLAGS) -o $(NAME)
	@echo "\033[0;32mSUCCESS !\033[0m \033[0;33m$(NAME)\033[0m"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "\033[0;34mDeleting almost everything !\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;35mDeleting everything !\033[0m"

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