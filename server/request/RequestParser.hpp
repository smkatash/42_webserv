#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "Request.hpp"

class	RequestParser {
	private:
		Request req_;

		void	getParam_(std::string& str, std::string& dest);
		void	parseRequestLine_(std::string line);
		void	parseGeneralHeader_(std::string line);
		void	parseRequestHeader_(std::string line);
		void	parseEntityHeader_(std::string line);
		void	parseMultiParamContent(std::string line);
		void	parseContentDisposition(std::string line);

	public:
		RequestParser();
		~RequestParser();
		void	initParser(std::string input);
		Request	getRequest();
		void	debug();
};


bool	isRequestLine(std::string line);
bool	isGeneralHeader(std::string line);
bool	isRequestHeader(std::string line);
bool	isEntityHeader(std::string line);
bool	startsWith(const std::string& line, const std::string& prefix);
bool	isBoundary(std::string line, std::string boundary);

#endif
