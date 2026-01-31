# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/05 17:00:00 by nofanizz          #+#    #+#              #
#    Updated: 2026/01/31 11:19:54 by nofanizz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/05 17:00:00 by nofanizz          #+#    #+#              #
#    Updated: 2026/01/30 09:24:17 by mvachon          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = webserv
CXX         = c++
CXXFLAGS    = -g3 -Wall -Wextra -Werror -std=c++98
RM          = rm -rf

INCLUDES    = -I includes

SRCS        = srcs/main.cpp

#include srcs/CGI/cgi.mk
include srcs/Exceptions/exceptions.mk
include srcs/ConfigParser/configparser.mk
include srcs/Management/management.mk
include srcs/Request/request.mk

BUILD_DIR   = .build
OBJS        = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS        = $(OBJS:.o=.d)

GREEN       = \033[0;32m
RED         = \033[0;31m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@
	@echo "$(GREEN)✓ Compiled: $<$(RESET)"

-include $(DEPS)

clean:
	@$(RM) $(BUILD_DIR)
	@echo "$(RED)✗ Object files and dependencies removed$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)✗ $(NAME) removed$(RESET)"

re: fclean all

.PHONY: all clean fclean re