#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "../requestHandler/Request.hpp"
#include "../parse/ConfigFile.hpp"

#define MAX_PATH_LEN 256
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

class CGIHandler {
	private:
		Request req_;
		ConfigFile conf_;
		std::string ep_;
		WebservCGI		cgi_;

		void	getRequestInfo();
		void	getConfigInfo();

	public:
		CGIHandler(Request req, ConfigFile conf, std::string location);
		~CGIHandler();
		void	setEnvironment();
		void	execute();
		void	runChildProcess(int fd, char** argv);
		void	runParentProcess(int status, int fd);
};


int			check_access(const char* file);
std::string	getAbsolutePath(std::string rootPath, std::string scriptPath);
char**		setArgArray(std::string cgiPath, std::string scripPath);
void		freeArgArray(char** argv);