/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:00 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/31 15:34:08 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

#define BUFFER_SIZE 50

class Request
{
    private:
			std::string method;		// GET POST DELETE
			std::string path;		// file
			std::string query;		// args for cgi script
			std::string version;	// HTTP/1.1 usually
			std::map<std::string, std::string> headers;	//headers
			std::string body;		// body content

		public:

        Request() {}
        ~Request() {}
		
        void CheckRequest();
        void Parse(const std::string &raw);
        
        bool IsComplete(std::string &request);
        void RequestReading(int &fd, bool &closedStatus, std::string &request);

        std::string GetPath() { return path; }
        std::string GetMethod() { return method; }
        std::string GetVersion() { return version; }
        std::map<std::string, std::string> GetHeaders() { return headers; }
        std::string GetBody() { return body; }

        void SetPath(std::string &str) { path = str; }
        
        static std::string headerFind(const std::map<std::string, std::string>& hdrs, 
                                       const std::string& key) {
            std::map<std::string, std::string>::const_iterator it = hdrs.find(key);
            return (it != hdrs.end()) ? it->second : "";
        }
};

#endif