#include "utils.hpp"

bool	isdigitString(std::string str) {
	if (str.empty())
		return false;
	for (unsigned long c = 0; c < str.length(); c++) {
		if (!isdigit(str[c]))
			return false;
	}
	return true;
}

bool	stripBrackets(std::string *str) {
	if (!(*str).empty() && (*str).back() == ';') {
		(*str).erase((*str).end() - 1);
		return true;
	}
	return false;
}

int	isValidMethod(std::string m) {
	if (m == "GET") 
		return GET;
	else if (m == "POST")
		return POST;
	else if (m == "DELETE")
		return DELETE;
	else
		throw std::invalid_argument("invalid method");
}

void	isValidLanguage(std::string lang) {
	if (lang.empty() || ( lang != ".py" && lang != ".php")) 
		throw std::invalid_argument("language not supported");
}