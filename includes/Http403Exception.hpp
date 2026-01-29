#ifndef HTTP403EXCEPTION_HPP
#define HTTP403EXCEPTION_HPP

#include "HttpException.hpp"

class Http403Exception : public HttpException {
public:
    virtual ~Http403Exception() throw() {}

    int getStatusCode() const { return 403; }

    const std::string& getStatusText() const {
        static std::string text = "Forbidden";
        return text;
    }

    const std::string& getPage() const {
        static std::string page = "website/error403Forbidden.html";
        return page;
    }
};

#endif
