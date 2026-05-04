# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    cgi.mk                                             :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmarcucc <lmarcucc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/31 11:14:01 by nofanizz          #+#    #+#              #
#    Updated: 2026/05/04 15:09:26 by lmarcucc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Ajout des sources CGI
SRCS += srcs/CGI/srcs/CgiManager.cpp \
		srcs/CGI/srcs/CgiWriter.cpp

# Ajout des includes CGI
INCLUDES += -Isrcs/CGI/includes
