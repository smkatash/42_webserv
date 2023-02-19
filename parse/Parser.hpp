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


		void	setConfigFile();
		void	debugInput();

};


#endif