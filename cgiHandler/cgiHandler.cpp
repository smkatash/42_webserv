#include "cgiHandler.hpp"

CGIHandler::CGIHandler(Request req, ConfigFile conf, std::string ep):
	req_(req), conf_(conf), ep_(ep) {
	// get data from Request and ConfigFile into CGI struct
	getRequestInfo();
	getConfigInfo();
};

CGIHandler::~CGIHandler() {};

void	CGIHandler::setEnvironment() {
	// set key/value args for cgi to get them from the environment
	setenv("REQUEST_METHOD", cgi_.method.c_str(), 1);
	setenv("CONTENT_TYPE", cgi_.contenType.c_str(), 1);
	setenv("CONTENT_LENGTH", cgi_.contenLength.c_str(), 1);
	setenv("HTTP_USER_AGENT", cgi_.userAgent.c_str(), 1);
	setenv("SCRIPT_FILENAME", cgi_.epScriptRoot.c_str(), 1);
	setenv("PATH_INFO", cgi_.cgiPathInfo.c_str(), 1);
	setenv("SERVER_NAME", cgi_.serverName.c_str(), 1);
	setenv("REDIRECT_STATUS", "200", 1);
}

void	CGIHandler::getRequestInfo() {
	cgi_.method = req_.rline.method;
	cgi_.uri =  req_.rline.uri;
	cgi_.contenType = req_.eheader.contentType;
	cgi_.contenLength = req_.eheader.contentLength;
	cgi_.userAgent = req_.rheader.userAgent;
}

void	CGIHandler::getConfigInfo() {
	// TODO should define path depending on the extension!!! This is only for php
	std::string	type = ".php";
	cgi_.cgiPathInfo =  getAbsolutePath(conf_.getRoot(ep_), "/cgi-bin/php-cgi");
	cgi_.epScriptRoot = getAbsolutePath(conf_.getRoot(ep_), conf_.getScriptCGI(ep_, type));
	cgi_.serverName = conf_.getServerName();
}


void	CGIHandler::execute() {
	pid_t	pid;
	int		status = 0;

	// set temp file to write into, for debugging, later will be replaced by fd[1]
	// and fd[0] for reading incomming request body
	int fd = open("dummy.html", O_RDWR | O_CREAT, 0777);
	if (fd == -1) {
		std::cout << "500 Server Error" << std::endl;
		return;
	}
	// transform string into array, memory is malloced, freed at the end of function
	char **argv = setArgArray(cgi_.cgiPathInfo, cgi_.epScriptRoot);
	pid = fork();
	if (pid == -1)
		throw std::runtime_error("Failed to create a process");
	else if (pid == 0)
		runChildProcess(fd, argv);
	else
		runParentProcess(status, fd);
	freeArgArray(argv);
}


void	CGIHandler::runChildProcess(int fd, char** argv) {
	// update enviroment with predefied arguments
	setEnvironment();
	extern char** environ;
	// write into fd aka temp file
	dup2(fd, STDOUT_FILENO);
	//check if +x exists
	if (!check_access(argv[0]) || !check_access(argv[1])) {
			throw std::runtime_error("Unauthorized");
	}
	if (execve(argv[0], argv, environ) == -1) {
		std::cerr << "execve failed: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	CGIHandler::runParentProcess(int status, int fd) {
	waitpid(1, &status, 0);
	if (!WIFSIGNALED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = WTERMSIG(status) + 128;
	// TODO how do we handle exit status? For now, I just set errno
	errno = status;
	std::cout << "Status: "<< status << std::endl;
	std::cout << "Got child back!" << std::endl;
	close(fd);
}
