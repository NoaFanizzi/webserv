/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:29:22 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/28 19:48:54 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>

class ExceptionPage : public std::exception {
    private:
        std::string _message;
    public:
        explicit ExceptionPage(const std::string& message) : _message(message) {}
        virtual ~ExceptionPage() throw() {}
        virtual const char* what() const throw() {
            return _message.c_str();
        }
};

class RequestParser
{
    private:
        std::string _method;
        std::string _url;
        std::string _version;
        static std::vector<std::string> _allrequest;
        static std::map<std::string, std::string> SeparateHeaders(std::vector<std::string> &docRequest);
        

    public:
        void CheckRequest();
        void ParseRequest(const std::string& request);
        void printRequest(std::vector<std::string> docRequest);
        RequestParser() {}
        ~RequestParser() {}
};

#endif
