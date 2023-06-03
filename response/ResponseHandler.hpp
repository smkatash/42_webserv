#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "Parser.hpp" // Methods enum

#define HTTPVERSION "HTTP/1.1"
#define SIDPATH     "./authentication_db/session_ids"

class ResponseHandler
{
private:
	Response    res_;
	Request     req_;
	ConfigFile  conf_;

	std::string uri_;
	std::string endpoint_;
	t_endpoint  location_;

	/* Response Generating */
	std::string responseLine();
	std::string generalHeader();
	std::string responseHeader();
	std::string entityHeader();
	void        setBodyErrorPage(int code);

	/* Response Handling */
	void        processCGIResponse(std::string& cgiResponse);
	bool        checkRequest();
	bool        isMethodAllowed(Methods method);
	void        setCode(int code);
	std::string findUriEndpoint(const std::string& uri);
	void        prepUriFile();
	void        setResponseBody(std::string fileName);
	void        returnResponse();
	void        autoIndexResponse(t_endpoint loc, std::string ep);
	void        dirResponse();
	void        normalResponse();

	// TODO: Not clean needs fixing
	void        normalDelResponse();
	void        dirDelResponse();

	/* Session Handling */
	void        addToSessionIds(const std::string& id);
	bool        validCookie();
	bool        authorized(std::string authorization);
	bool        authenticated();
	void        authenticate();

	/* Methods */
	void        get();
	void        post();
	void        del();

public:
	ResponseHandler(Request req, ConfigFile conf);
	~ResponseHandler();

	void        handle();

	std::string generate();
};

std::string     initAutoIndex(std::string endpoint, std::string root);

#endif
