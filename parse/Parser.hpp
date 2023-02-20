#ifndef PARSER_HPP
#define PARSER_HPP

#include "utils.hpp"

#define	SERVERNAMERROR "parser: server name invalid"


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

		Token	getToken(const std::string& str);
		void	parseBrackets();
		void	parseSyntax();

		int			checkPort(std::string p);
		std::string	checkServerName(std::string s);
		std::string checkRoot(std::string r);
		std::string checkIndexFile(std::string i);
		int			checkErrorCode(std::string errorCode);
		std::string	checkErrorPage(std::string errorPage);
		void		checkLocation(std::string endpoint);
		std::string	checkPath(std::string p);
		bool		checkAutoIndex(std::string indx);
		bool		endDirectiveLocation(std::string next, std::string *locationDir);
		void		setConfigFile();
		void		debugInput();

};


#endif