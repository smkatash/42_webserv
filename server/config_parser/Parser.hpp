#ifndef PARSER_HPP
#define PARSER_HPP

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

#define NOTFOUND		404
#define	INTERNALERROR	500

enum Token {
	SERVER,
	LISTEN,
	SERVER_NAME,
	ROOT,
	INDEX,
	ERROR_PAGE,
	LOCATION,
	METHOD,
	CGI,
	AUTOINDEX,
	BR,
	REDIRECT,
	MAX_CLIENT,
	AUTH,
	AUTH_FILE,
	NA
};

enum Methods {
	GET = 1,
	POST,
	DELETE
};

class Parser {
	private:
		std::vector<std::string>	input_;
		std::vector<ConfigFile>		conf_;
		int							count_;

	public:
		Parser();
		Parser(std::ifstream &file);
		~Parser();

		void			openFile(std::ifstream &file);

		/* parse */
		Token			getToken(const std::string& str);
		void			parseBrackets();
		void			parseSyntax();

		/* syntax check */
		std::string		checkServerName(std::string s);
		std::string 	checkRoot(std::string r);
		std::string 	checkIndexFile(std::string i);
		std::string		checkErrorPage(std::string errorPage);
		std::string		checkPath(std::string p);
		int				checkErrorCode(std::string errorCode);
		int				checkPort(std::string p);
		void			checkLocation(std::string endpoint);
		void			setConfigFile();
		std::vector<ConfigFile>		getArrayConfigFile();
		bool			checkAutoIndex(std::string indx);
		bool			endDirectiveLocation(std::string next, std::string *locationDir);
		std::string		checkRedirect(std::string redir);
		unsigned long	isValidLimit(std::string num);
		std::string		checkAuth(std::string auth);
		std::string		checkAuthUserFile(std::string path);

		/* debug */
		void			debugConfigVector();
		void			debugInput();
};

bool	isdigitString(std::string str);
bool	stripBrackets(std::string *str);
int		isValidMethod(std::string m);
void	isValidLanguage(std::string lang);

#endif