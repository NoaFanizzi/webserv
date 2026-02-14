#pragma once

#include "Request.hpp"
#include <string>
#include <vector>

class CgiManager
{
  private:
    // variables
    std::vector<std::string> _env;
    const Request &_request;
    std::string _scriptPath;
    std::string _output;

    // function
    void buildEnv();
    char **envToCharArray(); // used by function execv

  public:
    // constructor
    CgiManager(const Request &req, const std::string &scriptPath);
    ~CgiManager();

    // static function
    static bool isCgi(std::string path);

    // function
    bool execute();

    // getter
    std::string getOutput() const;
};
