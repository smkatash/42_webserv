#include "CGIHandler.hpp"

CGIHandler::CGIHandler(Request req, ConfigFile conf, std::string ep)
: req_(req)
, conf_(conf)
, ep_(ep)
{
	// get data from Request and ConfigFile into CGI struct
	setRequestInfo();
	setConfigInfo();
}

CGIHandler::CGIHandler(Request req, ConfigFile conf, std::string ep, std::string query)
: req_(req)
, conf_(conf)
, ep_(ep)
{
	// get data from Request and ConfigFile into CGI struct
	setRequestInfo();
	setConfigInfo();
	cgi_.queryString = query;
}

CGIHandler::~CGIHandler() {};

void	CGIHandler::setEnvironment()
{
	setenv("REQUEST_METHOD", cgi_.method.c_str(), 1);
	setenv("CONTENT_TYPE", cgi_.contenType.c_str(), 1);
	setenv("CONTENT_LENGTH", cgi_.contenLength.c_str(), 1);
	setenv("HTTP_USER_AGENT", cgi_.userAgent.c_str(), 1);
	setenv("SCRIPT_FILENAME", cgi_.epScriptRoot.c_str(), 1);
	setenv("PATH_INFO", cgi_.cgiPathInfo.c_str(), 1);
	setenv("FILE_NAME", cgi_.fileName.c_str(), 1);
	setenv("CONTENT_TYPE_FILE", cgi_.fileContentType.c_str(), 1);
	setenv("SERVER_NAME", cgi_.serverName.c_str(), 1);
	setenv("QUERY_STRING", cgi_.queryString.c_str(), 1);
	setenv("SESSION", cgi_.session.c_str(), 1);
	setenv("REDIRECT_STATUS", "200", 1);
}

void	CGIHandler::setRequestInfo()
{
	cgi_.method = req_.rline.method;
	cgi_.uri =  req_.rline.uri;
	cgi_.contenType = req_.eheader.contentType;
	cgi_.fileContentType = req_.eheader.fileContentType;
	cgi_.fileName = req_.eheader.fileName;
	cgi_.contenLength = req_.eheader.contentLength;
	cgi_.userAgent = req_.rheader.userAgent;
	cgi_.body = req_.rbody;
	cgi_.session = req_.rheader.cookie;
	cgi_.postType = (cgi_.contenType == MULTIPART) ? 0 : 1;
}

void	CGIHandler::setConfigInfo()
{
	int type = conf_.getScriptType(ep_);
	cgi_.cgiPathInfo =  getCgiAbsolutePath(type);
	cgi_.epScriptRoot = getAbsolutePath(conf_.getRoot(ep_), conf_.getScriptCGI(ep_, type));
	cgi_.serverName = conf_.getServerName();
}

void	CGIHandler::execute() {
	int	fd[2];
	char **argv = setArgArray(cgi_.cgiPathInfo, cgi_.epScriptRoot);
	responseFile_ = tmpfile();
	if (responseFile_ == nullptr) {
		throw std::runtime_error("Failed to create a temporary file");
	}

	if (pipe(fd) == -1)
		throw std::runtime_error("Failed to pipe");

	pid_t pid = fork();
	if (pid == -1) {
		freeArgArray(argv);
		throw std::runtime_error("Failed to create a process");
	}

	if (pid == 0)
		runChildProcess(fd, argv);
	else
		runParentProcess(fd);
	setCGIResponse();
}


void CGIHandler::setFileUpload() {
	if (std::string(getenv("CONTENT_TYPE")) == MULTIPART) {
		std::string tmpPath = getAbsolutePath(PHP_ROOT, "uploaded_file.tmp");
		std::ofstream tempFile(tmpPath, std::ios::binary);
		if (tempFile) {
			tempFile.write(&cgi_.body[0], cgi_.body.size());
			if (tempFile.fail()) {
				tempFile.close();
				throw std::runtime_error("Failed to write file");
			}
			tempFile.close();
			setenv("UPLOADED_FILE_PATH", tmpPath.c_str(), 1);
		}
	}
}

void	CGIHandler::runChildProcess(int *fd, char** argv)
{
	close(fd[1]);
	setEnvironment();
	setFileUpload();

	if (dup2(fd[0], STDIN_FILENO) == -1)
		std::runtime_error("Failed to redirect stdout");

	int filefd = fileno(responseFile_);
	if (filefd == -1) {
		close(filefd);
		throw std::runtime_error("Failed to retrieve file descriptor");
	}

	if (dup2(filefd, STDOUT_FILENO) == -1) {
		throw std::runtime_error("Failed to redirect stdout");
	}

	close(fd[0]);
	close(filefd);
	if (!check_access(argv[0]) || !check_access(argv[1]))
		throw std::runtime_error("Unauthorized");

	extern char** environ;
	if (execve(argv[0], argv, environ) == -1) {
		std::cerr << "execve failed: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CGIHandler::runParentProcess(int *fd)
{
	int status = 0;
	
	close(fd[0]);
	if (cgi_.postType && !cgi_.body.empty())
		write(fd[1], cgi_.body.c_str(), cgi_.body.length());
	close(fd[1]);

	waitpid(-1, &status, 0);
	if (!WIFSIGNALED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = WTERMSIG(status) + 128;
	if (status != 0) {
		throw std::runtime_error("execution failed");
	}
}

void	CGIHandler::setCGIResponse()
{
	std::ostringstream oss;
	char buffer[1024];

	rewind(responseFile_);
	while (fgets(buffer, sizeof(buffer), responseFile_) != NULL) {
		oss << buffer;
	}
	fclose(responseFile_);
	cgiResponse_ = oss.str();
}

std::string	CGIHandler::getCGIResponse()
{
	return cgiResponse_;
}

