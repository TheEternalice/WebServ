# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/14 15:47:01 by lde-merc          #+#    #+#              #
#    Updated: 2025/10/03 14:32:24 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

vpath %.cpp srcs 
vpath %.conf conf_file

NAME = webserv

CXX = c++
CXXFLAGS = -MMD -Wall -Werror -Wextra -g -std=c++98

INCLUDES = includes/
OBJ_DIR = objs/
SRC_DIR = srcs/
DEP		:= $(OBJ:.o=.d)

INVALID_FILE = empty_file.conf wrong_extension.txt

# Liste des fichiers source
SRC_FILES = main.cpp HTTP_Server.cpp Server_parsing.cpp Reponse.cpp Socket.cpp
			
# Transforme chaque fichier source en un fichier objet dans $(OBJ_DIR)
OBJS = $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

all: mkdir_obj $(NAME)

# Compilation des fichiers .cpp en .o dans le dossier $(OBJ_DIR)
$(OBJ_DIR)%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Création du dossier objs/ et des sous-répertoires s'ils n'existent pas
mkdir_obj:
	@mkdir -p $(OBJ_DIR)

# Compilation finale
$(NAME): $(OBJS)
	@echo "\033[34mCompilation $(NAME) en cours\033[0m"
	@$(CXX) $(OBJS) $(CXXFLAGS) -o $(NAME)
	@echo "\033[0;32mSUCCESS !\033[0m \033[0;33m$(NAME)\033[0m"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "\033[0;34mDeleting almost everything !\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;35mDeleting everything !\033[0m"

re: fclean all

-include $(DEP)

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
