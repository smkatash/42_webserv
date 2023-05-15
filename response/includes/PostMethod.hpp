#ifndef POSTMETHOD_HPP
#define POSTMETHOD_HPP

#include "ResponseGenerator.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"

class PostMethod : virtual public ResponseGenerator
{
private:
	std::string	findUriEndpoint(const std::string& uri);
	void		prepUriFile(std::string& uri, const t_endpoint& loc);
	void		setResponseBody(std::string fileName);
	void		returnResponse(t_endpoint loc);
	void		dirResponse(t_endpoint loc, std::string ep);
	void		normalResponse(t_endpoint loc, std::string uri);
public:
	PostMethod(Request& req, ConfigFile& conf);
	~PostMethod();

	void	post();
};


#endif
