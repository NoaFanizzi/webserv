#ifndef HTTP404EXCEPTION_HPP
#define HTTP404EXCEPTION_HPP

#include "HttpException.hpp"

class Http404Exception : public HttpException {
public:
    virtual ~Http404Exception() throw() {}

    int getStatusCode() const { return 404; }

    const std::string& getStatusText() const {
        static std::string text = "Not Found";
        return text;
    }

    const std::string& getPage() const {
        static std::string page = "website/error404NotFound.html";
        return page;
    }
};

#endif
