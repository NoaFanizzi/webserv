# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    cgi.mk                                             :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmarcucc <lmarcucc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/31 11:14:01 by nofanizz          #+#    #+#              #
#    Updated: 2026/05/11 15:16:16 by lmarcucc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Ajout des sources CGI
SRCS += srcs/CGI/srcs/CgiManager.cpp \
		srcs/CGI/srcs/CgiWriter.cpp \
		srcs/CGI/srcs/CgiReader.cpp

# Ajout des includes CGI
INCLUDES += -Isrcs/CGI/includes
