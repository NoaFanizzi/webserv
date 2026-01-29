/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:29:22 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/29 14:54:05 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>

#define BUFFER_SIZE 2

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
        std::string GetUrl() {return _url;}
        void CheckRequest();
        void ParseRequest(const std::string& request);
        void printRequest(std::vector<std::string> docRequest, std::map<std::string, std::string> &headers);
        void RequestReading(int &fd, bool &closedStatus, std::string &request);
        bool IsComplete(std::string &request);
        RequestParser() {}
        ~RequestParser() {}
};

#endif
