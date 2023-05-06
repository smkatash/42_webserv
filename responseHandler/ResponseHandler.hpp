#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include <iostream>
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "parser_utils.hpp" // Methods enum

std::string	initAutoIndex(std::string endpoint, std::string root);

class ResponseHandler {
private:
	Response	res_;
	Request		req_;
	ConfigFile	conf_;

	std::string	responseLine();
	std::string	generalHeader();
	std::string	responseHeader();
	std::string	entityHeader();


	bool		isMethodAllowed(Methods method, std::vector<int> methods);
	std::string	findUriEndpoint(const std::string& uri);

	void		prepUriFile(std::string& uri, const t_endpoint& loc);
	void		setResponseBody(std::string fileName);

	void		getReturnResponse(t_endpoint loc);
	void		getAutoIndexResponse(t_endpoint loc, std::string ep);
	void		getDirResponse(t_endpoint loc, std::string ep);
	void 		getNormalResponse(t_endpoint loc, std::string uri);

	void		setCode(int code);
public:
	ResponseHandler(Request req, ConfigFile conf);
	~ResponseHandler();

	std::string	getResponse();

	void		get();
	void		post();
	void		del();
	void		head();
};

#endif
