#ifndef HTTPEXCEPTION_HPP
#define HTTPEXCEPTION_HPP

#include <exception>
#include <string>

class HttpException : public std::exception {
public:
    virtual ~HttpException() throw() {}

    virtual int getStatusCode() const = 0;
    virtual const std::string& getStatusText() const = 0;
    virtual const std::string& getPage() const = 0;
};

#endif
