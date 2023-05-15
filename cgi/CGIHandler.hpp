#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "Request.hpp"
#include "ConfigFile.hpp"

#define MAX_PATH_LEN 512
#define PHP_CGI_PATH "/exec/php-cgi"
#define PHP_ROOT "/cgi"
/*
A query string is typically used to pass data to the server via an HTTP GET request.
The query string is appended to the end of the URL in the format ?key1=value1&key2=value2..., 
where each key-value pair is separated by an ampersand (&). 
The server can then extract the data from the query string and use it to generate a response. 
Query strings are commonly used for things like search queries and filtering parameters.

On the other hand, input is typically used to pass data to the server via an HTTP POST 
request. The input data is included in the body of the request, which can be of any content 
type (e.g. JSON, XML, URL-encoded form data, etc.). The server can then extract the data 
from the request body and use it to generate a response. POST requests are commonly used for things 
like submitting forms, uploading files, and making API calls.
*/

struct cgi_handler {
	std::string	method;
	std::string contenType;
	std::string contenLength;
	std::string userAgent;
	std::string	serverName;
	std::string uri;
	std::string epScriptRoot;
	std::string cgiPathInfo;
	std::string queryString;
	std::string body;
};

typedef cgi_handler WebservCGI;

class CGIHandler
{
private:
	Request				req_;
	ConfigFile			conf_;
	std::string			ep_;
	WebservCGI			cgi_;
	std::string			cgiResponse_;

	void				getRequestInfo();
	void				getConfigInfo();

	void				setEnvironment();
	void				runChildProcess(int *fd, char** argv);
	void				runParentProcess(int *fd);
	// void				runChildProcess(int stdIn, int stdOut, char** argv);
	// void				runParentProcess(int stdOut);
	void				setCGIResponse(char* tmpname);
public:
	CGIHandler(Request req, ConfigFile conf, std::string location);
	CGIHandler(Request req, ConfigFile conf, std::string location, std::string query);
	~CGIHandler();

	void				execute();
	std::string			getCGIResponse();
};

int						check_access(const char* file);
std::string				getAbsolutePath(std::string rootPath, std::string scriptPath);
char**					setArgArray(std::string cgiPath, std::string scripPath);
void					freeArgArray(char** argv);

int			check_access(const char* file);
std::string	getAbsolutePath(std::string rootPath, std::string scriptPath);
char**		setArgArray(std::string cgiPath, std::string scripPath);
void		freeArgArray(char** argv);

#endif