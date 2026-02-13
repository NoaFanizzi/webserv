#pragma once

#include "Request.hpp"
#include "AManager.hpp"
#include <string>
#include <vector>

class CgiManager : public AManager {
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
  CgiManager();
  ~CgiManager();

  // static function
  static bool isCgi(std::string path);

  // function
  void PollInHandler();
  void PollOutHandler();
  bool execute();

  // getter
  std::string getOutput() const;
};
