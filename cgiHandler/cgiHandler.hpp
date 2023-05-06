#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include "../requestHandler/Request.hpp"
#include "../parse/ConfigFile.hpp"

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
	const char*	serverName;
	const char*	method;
	const char* contenType;
	const char* contenLength;
	const char* userAgent;
	const char* fullpathURI;
	const char* cgiScriptPath;
	const char* queryString;
};

typedef cgi_handler webservCGI;

class CGIHandler {
	private:
		Request req_;
		ConfigFile conf_;
		// dummy, i think it should be vector or map
		std::string queryString_;
		std::string inputBody_;
		// get from config?
		std::string scriptPath_;
		std::string	serverName_;

		webservCGI		cgi_;
		void	initialize();

	public:
		CGIHandler(Request req);
		~CGIHandler();
		void	setEnvironment();
};

