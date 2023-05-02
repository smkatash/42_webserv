#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include <iostream>
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "parser_utils.hpp" // Methods enum

class ResponseHandler {
private:
	Response	res_;
	Request		req_;
	ConfigFile	conf_;

	std::string	appendResponseLine();
	std::string	appendGeneralHeader();
	std::string	appendResponseHeader();
	std::string	appendEntityHeader();


	bool		isMethodAllowed(Methods method, std::vector<int> methods);
	std::string	getUriEndpoint(const std::string& uri);

	void		prepUriFile(std::string& uri, const t_endpoint& loc);


	void		setResponseBody(std::string fileName);
	void		directoryRequest(const t_endpoint& loc);

	void		setCode(std::string code);
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
