#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <sstream>

bool	isRequestLine(std::string line);
bool	isGeneralHeader(std::string line);
bool	isRequestHeader(std::string line);
bool	isEntityHeader(std::string line);

#endif