/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:35:57 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/06 17:21:06 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <filesystem>
#include <iostream>
#include <vector>


class AutoIndex
{
	private:
		static std::string _header;
		static std::string _template;
		std::string _content;
		static std::string _footer;
	public:

		//methods
		std::string initAutoIndex();
		std::string replaceTemplate(struct dirent &sdir, const std::string &root);
		void addNewRow(struct dirent &sdir);
		void replaceName(std::string &newTemplate, struct dirent &sdir);
		void replaceLink(std::string &newTemplate, std::string &filepath);
		void replaceDate(std::string &newTemplate, struct stat &file);
		void replaceWeight(std::string &newTemplate, struct stat &file);

};

#endif


//-------------------------------------------FIRST_PART

// <!doctype html>
// <html lang='fr'>
//   <style>
//     .tab {
//       display:grid;
//       grid-template-columns: repeat(3, auto);
//     }
//     .tab > div {

//     }
//     .column {
//       border: solid;
//     }
//     .column > span{
//       display: block;
//       background-color: red;
//       text-align: center;
//     }
//   </style>

//   <head>
//     <meta charset='utf-8' />
//     <title>Titre de la page</title>
//     <link rel='stylesheet' href='style.css' />
//     <script src='script.js'></script>
//   </head>
//   <body>
//     <section>
//       <div class='tab'>

//--------------------------------------------LAST_PART
//     </section>
//   </body>
// </html>