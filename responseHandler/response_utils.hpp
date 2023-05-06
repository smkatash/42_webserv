#ifndef RESPONSE_UTILS
#define RESPONSE_UTILS

#include <iostream>
#include <vector>
#include <sstream>

std::string findCurrentTimeGMT();

std::string	findContentType(std::string fileExtension);

std::string	findUsableFile(std::vector<std::string> files, std::string directory);

bool		isNumber(std::string str);

template <typename T>
std::string toString(const T& value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

int			strtoi(std::string const& str);

bool		Pred(char a, char b);
std::string	removeDuplicateSlashes(const std::string& str);

#endif
