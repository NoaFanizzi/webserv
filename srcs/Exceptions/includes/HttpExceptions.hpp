/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpExceptions.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:24:45 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/05 08:34:27 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPEXCEPTIONS_HPP
#define HTTPEXCEPTIONS_HPP

#include <exception>
#include <string>

class HttpException : public std::exception {
public:
    virtual ~HttpException() throw() {}

    virtual int getStatusCode() const = 0;
    virtual const std::string& getStatusText() const = 0;
};

class Http400Exception : public HttpException {
public:
    virtual ~Http400Exception() throw() {}

    int getStatusCode() const { return 400; }

    const std::string& getStatusText() const {
        static std::string text = "Bad Request";
        return text;
    }
};

class Http404Exception : public HttpException {
public:
    virtual ~Http404Exception() throw() {}

    int getStatusCode() const { return 404; }

    const std::string& getStatusText() const {
        static std::string text = "Not Found";
        return text;
    }
};

class Http403Exception : public HttpException {
public:
    virtual ~Http403Exception() throw() {}

    int getStatusCode() const { return 403; }

    const std::string& getStatusText() const {
        static std::string text = "Forbidden";
        return text;
    }
};

class Http405Exception : public HttpException {
public:
    virtual ~Http405Exception() throw() {}

    int getStatusCode() const { return 405; }

    const std::string& getStatusText() const {
        static std::string text = "Method Not Allowed";
        return text;
    }
};

class Http408Exception : public HttpException {
public:
    virtual ~Http408Exception() throw() {}

    int getStatusCode() const { return 408; }

    const std::string& getStatusText() const {
        static std::string text = "Request Timeout";
        return text;
    }
};

class Http500Exception : public HttpException {
public:
    virtual ~Http500Exception() throw() {}

    int getStatusCode() const { return 500; }

    const std::string& getStatusText() const {
        static std::string text = "Internal Error";
        return text;
    }
};


#endif
