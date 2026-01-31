#pragma once

#include <string>
#include <vector>
#include "Request.hpp"

class RequestParser
{
public:
	static Request Parse(const std::string &raw);
};
