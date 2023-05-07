#ifndef PARSER_HPP
#define PARSER_HPP

#include "utils.hpp"

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
	NA
};

class Parser {
	private:
		std::vector<std::string>	input_;
		std::vector<ConfigFile>		conf_;
		int							count_;

	public:
		Parser(std::ifstream &file);
		~Parser();

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
		bool			checkAutoIndex(std::string indx);
		bool			endDirectiveLocation(std::string next, std::string *locationDir);
		std::string		checkRedirect(std::string redir);
		unsigned long	isValidLimit(std::string num);

		/* debug */
		void			debugConfigVector();
		void			debugInput();
};


#endif