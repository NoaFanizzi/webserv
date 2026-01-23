# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/05 17:00:00 by nofanizz          #+#    #+#              #
#    Updated: 2026/01/23 05:45:38 by mvachon          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = webserv
CXX         = c++
CXXFLAGS    = -g3 -Wall -Wextra -Werror -std=c++98
INCLUDES    = -I includes
RM          = rm -rf

SRCS_DIR    = srcs
SRCS        = main.cpp  Config.cpp DEBUG_PrintConfig.cpp Client.cpp Server.cpp ManageAll.cpp
SRCS_PATH   = $(addprefix $(SRCS_DIR)/, $(SRCS))

BUILD_DIR   = .build
OBJS        = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))
DEPS        = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.d))

HEADERS     =  Config.hpp Client.hpp ManageAll.hpp Server.hpp

GREEN       = \033[0;32m
RED         = \033[0;31m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(BUILD_DIR)
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
