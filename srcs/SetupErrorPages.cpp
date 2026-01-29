/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SetupErrorPages.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 19:00:51 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/29 19:22:04 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void Client::setErrorPages()
{
    // 404
    _errorPages[404] =
        "<!DOCTYPE html>\n"
        "<html lang=\"fr\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <title>404 - Page introuvable</title>\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <style>\n"
        "        body {\n"
        "            margin: 0;\n"
        "            font-family: Arial, Helvetica, sans-serif;\n"
        "            background: linear-gradient(135deg, #292e45ff, #764ba2);\n"
        "            color: #fff;\n"
        "            display: flex;\n"
        "            align-items: center;\n"
        "            justify-content: center;\n"
        "            height: 100vh;\n"
        "            text-align: center;\n"
        "        }\n"
        "        .container {\n"
        "            max-width: 500px;\n"
        "            padding: 40px;\n"
        "        }\n"
        "        h1 {\n"
        "            font-size: 120px;\n"
        "            margin: 0;\n"
        "            line-height: 1;\n"
        "        }\n"
        "        h2 {\n"
        "            margin: 10px 0 20px;\n"
        "            font-weight: normal;\n"
        "        }\n"
        "        p {\n"
        "            opacity: 0.9;\n"
        "            margin-bottom: 30px;\n"
        "        }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"container\">\n"
        "        <h1>404</h1>\n"
        "        <h2>Page introuvable</h2>\n"
        "        <p>Oups... La page que vous cherchez n'existe pas ou a ete deplacee.</p>\n"
        "    </div>\n"
        "</body>\n"
        "</html>\n";

    // 403
    _errorPages[403] =
        "<!DOCTYPE html>\n"
        "<html lang=\"fr\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <title>403 - Page Interdite</title>\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <style>\n"
        "        body {\n"
        "            margin: 0;\n"
        "            font-family: Arial, Helvetica, sans-serif;\n"
        "            background: linear-gradient(135deg, #2c3454ff, #764ba2);\n"
        "            color: #fff;\n"
        "            display: flex;\n"
        "            align-items: center;\n"
        "            justify-content: center;\n"
        "            height: 100vh;\n"
        "            text-align: center;\n"
        "        }\n"
        "        .container {\n"
        "            max-width: 500px;\n"
        "            padding: 40px;\n"
        "        }\n"
        "        h1 {\n"
        "            font-size: 120px;\n"
        "            margin: 0;\n"
        "            line-height: 1;\n"
        "        }\n"
        "        h2 {\n"
        "            margin: 10px 0 20px;\n"
        "            font-weight: normal;\n"
        "        }\n"
        "        p {\n"
        "            opacity: 0.9;\n"
        "            margin-bottom: 30px;\n"
        "        }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"container\">\n"
        "        <h1>403</h1>\n"
        "        <h2>Page Interdite</h2>\n"
        "        <p>Oups... Vous n'avez pas les droits d'aller sur cette page</p>\n"
        "    </div>\n"
        "</body>\n"
        "</html>\n";
}
