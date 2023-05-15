#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "parser_utils.hpp" // Methods enum

class ResponseHandler
{
private:
	Response	res_;
	Request		req_;
	ConfigFile	conf_;

	// Response Generating
	std::string	responseLine();
	std::string	generalHeader();
	std::string	responseHeader();
	std::string	entityHeader();
	void		setBodyErrorPage(int code);

	// Response Handling
	bool		isMethodAllowed(Methods method, std::vector<int> methods);
	void		setCode(int code);
	std::string	findUriEndpoint(const std::string& uri);
	void		prepUriFile(std::string& uri, const t_endpoint& loc);
	void		setResponseBody(std::string fileName);
	void		returnResponse(t_endpoint loc);
	void		autoIndexResponse(t_endpoint loc, std::string ep);
	void		dirResponse(t_endpoint loc, std::string ep);
	void		normalResponse(t_endpoint loc, std::string uri);

	// TODO: Not clean needs fixing
	void		normalDelResponse(t_endpoint loc, std::string uri);
	void		dirDelResponse(t_endpoint loc, std::string ep);


public:
	ResponseHandler(Request req, ConfigFile conf);
	~ResponseHandler();

	void		get();
	void		post();
	void		del();

	std::string	generateResponse();
};

std::string		initAutoIndex(std::string endpoint, std::string root);

#endif
