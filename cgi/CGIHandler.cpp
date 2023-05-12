#include "CGIHandler.hpp"

CGIHandler::CGIHandler(Request req, ConfigFile conf, std::string ep)
: req_(req)
, conf_(conf)
, ep_(ep)
{
	// get data from Request and ConfigFile into CGI struct
	getRequestInfo();
	getConfigInfo();
}

CGIHandler::CGIHandler(Request req, ConfigFile conf, std::string ep, std::string query)
: req_(req)
, conf_(conf)
, ep_(ep)
{
	// get data from Request and ConfigFile into CGI struct
	getRequestInfo();
	getConfigInfo();
	cgi_.queryString = query;
}

CGIHandler::~CGIHandler() {};

void	CGIHandler::setEnvironment()
{
	// set key/value args for cgi to get them from the environment
	setenv("REQUEST_METHOD", cgi_.method.c_str(), 1);
	setenv("CONTENT_TYPE", cgi_.contenType.c_str(), 1);
	setenv("CONTENT_LENGTH", cgi_.contenLength.c_str(), 1);
	setenv("HTTP_USER_AGENT", cgi_.userAgent.c_str(), 1);
	setenv("SCRIPT_FILENAME", cgi_.epScriptRoot.c_str(), 1);
	setenv("PATH_INFO", cgi_.cgiPathInfo.c_str(), 1);
	setenv("SERVER_NAME", cgi_.serverName.c_str(), 1);
	setenv("REDIRECT_STATUS", "200", 1);
	setenv("QUERY_STRING", cgi_.queryString.c_str(), 1);
}

void	CGIHandler::getRequestInfo()
{
	cgi_.method = req_.rline.method;
	cgi_.uri =  req_.rline.uri;
	cgi_.contenType = req_.eheader.contentType;
	cgi_.contenLength = req_.eheader.contentLength;
	cgi_.userAgent = req_.rheader.userAgent;
}

void	CGIHandler::getConfigInfo()
{
	// TODO should define path depending on the extension!!! This is only for php
	std::string	type = ".php";
	std::cout << ep_ << std::endl;
	std::string root = conf_.getRoot(ep_);
	std::string script = conf_.getScriptCGI(ep_, type);
	std::cout << root << std::endl;
	std::cout << script << std::endl;
	cgi_.cgiPathInfo =  getAbsolutePath(root, "/cgi-bin/php-cgi");
	cgi_.epScriptRoot = getAbsolutePath(root, script);
	std::cout << cgi_.cgiPathInfo << std::endl;
	std::cout << cgi_.epScriptRoot  << std::endl;
	cgi_.serverName = conf_.getServerName();
	exit(1);
}

void	CGIHandler::execute()
{
	/* Creating a temp file with a guaranteed new file */
	std::string tmpname = "/tmp/tmpfileXXXXXX";
	int filefd = mkstemp((char*)tmpname.c_str());

	/* To make script read from a different input */
	int	fd[2];
	if (pipe(fd) == -1)
		throw std::runtime_error("Failed to pipe");

	/* Transform string into array. Memory is malloced and freed at the end of function */
	char **argv = setArgArray(cgi_.cgiPathInfo, cgi_.epScriptRoot);

	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("Failed to create a process");
	if (pid == 0)
		runChildProcess(fd, filefd, argv);
	else
		runParentProcess(fd, (char*)tmpname.c_str());
	freeArgArray(argv);
	close(filefd);
}


void	CGIHandler::runChildProcess(int *fd, int filefd, char** argv)
{
	/* Update enviroment with predefied arguments */
	setEnvironment();
	extern char** environ;

	/* Read from pipe */
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
	close(fd[1]);

	/* Write into temp file */
	dup2(filefd, STDOUT_FILENO);
	close(filefd);

	/* Check if +x exists */
	if (!check_access(argv[0]) || !check_access(argv[1]))
		throw std::runtime_error("Unauthorized");
	if (execve(argv[0], argv, environ) == -1)
	{
		std::cerr << "execve failed: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	CGIHandler::setCGIResponse(char* tmpname)
{
	std::fstream file(tmpname);
	std::string buffer;
	while(std::getline(file, buffer))
		cgiResponse_ += buffer + '\n';
}

void	CGIHandler::runParentProcess(int *fd, char* tmpname)
{
	/* Write the request body if exists into pipe and close the latter */
	if (!cgi_.body.empty())
		write(fd[1], cgi_.body.c_str(), cgi_.body.size());
	close(fd[1]);
	close(fd[0]);

	/* Wait for child and catch status */
	int status = 0;
	waitpid(-1, &status, 0);
	if (!WIFSIGNALED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = WTERMSIG(status) + 128;
	if (status != 0)
		throw std::runtime_error("execution problem");

	/* Now we read what the script spits out */
	setCGIResponse(tmpname);
}

std::string	CGIHandler::getCGIResponse()
{
	return cgiResponse_;
}
