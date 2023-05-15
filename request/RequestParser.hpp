#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "Request.hpp"

class	RequestParser {
	private:
		Request req_;

		void	initParser_(std::string input);
		void	getParam_(std::string& str, std::string& dest);
		void	parseRequestLine_(std::string line);
		void	parseGeneralHeader_(std::string line);
		void	parseRequestHeader_(std::string line);
		void	parseEntityHeader_(std::string line);
		void	parseMultiParamContent(std::string line);
		void	parseContentDisposition(std::string line);

	public:
		RequestParser(std::string input);
		~RequestParser();
		Request	getRequest();
		void	debug();


	
};


#endif
