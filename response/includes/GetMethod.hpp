#ifndef GETMETHOD_HPP
#define GETMETHOD_HPP

#include <iostream>
#include "ResponseGenerator.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"

class GetMethod : virtual public ResponseGenerator
{
private:
	std::string	findUriEndpoint(const std::string& uri);
	void		prepUriFile(std::string& uri, const t_endpoint& loc);
	void		setResponseBody(std::string fileName);
	void		returnResponse(t_endpoint loc);
	void		autoIndexResponse(t_endpoint loc, std::string ep);
	void		dirResponse(t_endpoint loc, std::string ep);
	void		normalResponse(t_endpoint loc, std::string uri);
public:
	GetMethod(Request& req, ConfigFile& conf);
	~GetMethod();

	void		get();
};

std::string		initAutoIndex(std::string endpoint, std::string root);

#endif
