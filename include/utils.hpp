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

/* TODO most common error codes, may be add more later*/
#define	UNAUTHORIZED 	400		// "400 Unauthorized" /* unauthenticated request */
#define NOTFOUND		404		// "404 Not Found" /* requested page not found */
#define GONE			410		// "410 Gone" /* like 404, but temporary*/
#define	INTERNALERROR	500		// "500 Internal Server Error" /* generic server error */
#define	BADGATEWAY		502		// "502 Bad Gateway" /* invalid response from another server */
#define	MOVEDPERMAN		301		// "301 Moved Permanently" /* page moved to a different url*/
#define	FOUND			302		// "302 Found" /* used for temporary redirect */


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
int printError(std::string message, int ERROR_CODE);

#endif