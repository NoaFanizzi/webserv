# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/05 07:48:23 by mvachon           #+#    #+#              #
#    Updated: 2026/02/24 14:16:20 by mvachon          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = webserv
CXX         = c++
CXXFLAGS    = -g3 -Wall -Wextra -Werror -std=c++98
RM          = rm -rf

INCLUDES    = -I includes

SRCS        = srcs/main.cpp

include srcs/CGI/cgi.mk
include srcs/AutoIndex/AutoIndex.mk
include srcs/Exceptions/exceptions.mk
include srcs/ConfigParser/configparser.mk
include srcs/Management/management.mk
include srcs/RequestResponse/requestresponse.mk

BUILD_DIR   = .build
OBJS        = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS        = $(OBJS:.o=.d)

UPLOAD_FILE = upload

GREEN       = \033[0;32m
RED         = \033[0;31m
RESET       = \033[0m

all: $(UPLOAD_FILE) $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"

$(UPLOAD_FILE):
	@mkdir -p $(UPLOAD_FILE)
	@echo "$(GREEN)✓ upload file created$(RESET)"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@
	@echo "$(GREEN)✓ Compiled: $<$(RESET)"

-include $(DEPS)

run: all
	@echo "$(GREEN)▶ Launching $(NAME) with config.conf$(RESET)"
	@./$(NAME) config.conf

clean:
	@$(RM) $(BUILD_DIR)
	@echo "$(RED)✗ Object files and dependencies removed$(RESET)"

fclean: clean
	@$(RM) $(NAME) $(UPLOAD_FILE)
	@echo "$(RED)✗ $(NAME) and upload removed$(RESET)"

val:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes -s ./$(NAME) config.conf


re: fclean all

.PHONY: all clean fclean re
