#ifndef RESPONSEGENERATOR_HPP
#define RESPONSEGENERATOR_HPP

#include <iostream>
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "parser_utils.hpp" // Methods enum

class ResponseGenerator
{
protected:
	Response	res_;
	Request		req_;
	ConfigFile	conf_;

	bool		isMethodAllowed(Methods method, std::vector<int> methods);
	void		setCode(int code);

private:
	std::string	responseLine();
	std::string	generalHeader();
	std::string	responseHeader();
	std::string	entityHeader();

public:
	ResponseGenerator(Request& req, ConfigFile& conf);
	~ResponseGenerator();

	std::string	generateResponse();
};

#endif