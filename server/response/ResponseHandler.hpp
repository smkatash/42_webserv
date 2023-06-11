#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "Parser.hpp" // Methods enum

#define HTTPVERSION "HTTP/1.1"
#define SIDPATH     "server/authentication_db/session_ids"
#define AUTOINDEX_TEMPLATE_PATH "server/response/autoindex/template.html"

static const struct s_methods
{
	std::string methodStr;
	Methods     methodVal;
}
m[] =
{
	{"GET", GET},
	{"POST", POST},
	{"DELETE", DELETE}
};

class ResponseHandler
{
public:
	ResponseHandler(Request req, ConfigFile conf);
	~ResponseHandler();

	void        handle();
	std::string generate();

private:
	Response    res_;
	Request     req_;
	ConfigFile  conf_;

	std::string uri_;
	std::string endpoint_;
	t_endpoint  location_;

	/* Methods */
	void        get();
	void        post();
	void        del();

	/* Response Handling */
	void        processCGIResponse(std::string& cgiResponse);
	std::string findUriEndpoint(const std::string& uri);
	void        prepUriFile();
	void        setResponseBody(std::string fileName);
	void        returnResponse();
	void        autoIndexResponse(t_endpoint loc, std::string ep);
	void        dirResponse(Methods method);
	void        normalResponse(Methods method);

	/* Response Code Setting */
	void        setBodyErrorPage(int code);
	void        setCode(int code);

	/* Session Handling */
	void        addToSessionIds(const std::string& id);
	bool        validCookie();
	bool        authorized(std::string authorization);
	bool        authenticated();
	void        authenticate();

	/* Checks */
	bool        checkRequest();
	bool        checkMethod();
	bool        isMethodAllowed(Methods method);

	/* Response Generating */
	std::string responseLine();
	std::string generalHeader();
	std::string responseHeader();
	std::string entityHeader();
};

std::string     initAutoIndex(std::string endpoint, std::string root);

#endif
