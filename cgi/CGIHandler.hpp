#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <algorithm>
#include <vector>
#include "Request.hpp"
#include "ConfigFile.hpp"

#define MAX_PATH_LEN 512
#define PHP_CGI_PATH "/exec/php-cgi"
#define PYTHON_CGI_PATH "/exec/python"
#define PERL_CGI_PATH "/exec/perl"
#define PHP_ROOT "/cgi"
#define POST_FILE 0
#define POST_DATA 1

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
	std::string fileContentType;
	std::string fileName;
	std::string session;
	int			postType;
};

typedef cgi_handler WebservCGI;

class CGIHandler
{
private:
	Request				req_;
	ConfigFile			conf_;
	std::string			ep_;
	WebservCGI			cgi_;
	FILE*				responseFile_;
	std::string			cgiResponse_;

	void				setRequestInfo();
	void				setConfigInfo();
	void				setEnvironment();
	void				setFileUpload();

	void				runChildProcess(int *fd, char** argv);
	void				runParentProcess(int *fd);
	void				setCGIResponse();

public:
	CGIHandler(Request req, ConfigFile conf, std::string location);
	CGIHandler(Request req, ConfigFile conf, std::string location, std::string query);
	~CGIHandler();
	void				execute();
	std::string			getCGIResponse();
};

std::string				base64Decode(const std::string& input);
std::string				getCwd();
std::string				getCgiAbsolutePath(int type);
std::string				getAbsolutePath(std::string rootPath, std::string scriptPath);
int						check_access(const char* file);
char**					setArgArray(std::string cgiPath, std::string scripPath);
void					freeArgArray(char** argv);

#endif