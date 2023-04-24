#ifndef UTILS_HPP
#define UTILS_HPP

#include "ConfigFile.hpp"
#include <iostream>
#include <utility>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>

/* Methods*/
enum Methods {
	GET = 1,
	POST,
	DELETE
};

/* helpers */
bool	isdigitString(std::string str);
bool	stripBrackets(std::string *str);
int		isValidMethod(std::string m);
void	isValidLanguage(std::string lang);

#endif