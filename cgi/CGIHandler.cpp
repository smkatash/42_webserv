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
	if (!cgi_.queryString.empty())
		setenv("QUERY_STRING", cgi_.queryString.c_str(), 1);
}

void	CGIHandler::getRequestInfo()
{
	cgi_.method = req_.rline.method;
	cgi_.uri =  req_.rline.uri;
	cgi_.contenType = req_.eheader.contentType;
	cgi_.contenLength = req_.eheader.contentLength;
	cgi_.userAgent = req_.rheader.userAgent;
	cgi_.body = req_.rbody;
	cgi_.binbody = req_.binbody;
}

void	CGIHandler::getConfigInfo()
{
	// TODO should define path depending on the extension!!! This is only for php
	std::string	type = ".php";
	std::string root = conf_.getRoot(ep_);
	std::string script = conf_.getScriptCGI(ep_, type);

	cgi_.cgiPathInfo =  getAbsolutePath(PHP_ROOT, PHP_CGI_PATH);
	cgi_.epScriptRoot = getAbsolutePath(root, script);
	cgi_.serverName = conf_.getServerName();
}

void	CGIHandler::execute() {
	int	fd[2];
	char **argv = setArgArray(cgi_.cgiPathInfo, cgi_.epScriptRoot);
	
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
}


void CGIHandler::fileUpload() {
	if (std::string(getenv("CONTENT_TYPE")) == "multipart/form-data") {
		// Decode the binary data (assuming base64 encoding)
		std::vector<char> decodedData = base64Decode(cgi_.body);

		// Create a temporary file to save the decoded data
		std::string tmpPath = getAbsolutePath(PHP_ROOT, "uploaded_file.tmp");
		std::ofstream tempFile(tmpPath, std::ios::binary);
		if (tempFile) {
			// Write the decoded data to the temporary file
			tempFile.write(&decodedData[0], decodedData.size());
			if (tempFile.fail()) {
				tempFile.close();
				throw std::runtime_error("Failed to write file");
			}
			tempFile.close();
			// Set the environment variable with the path to the temporary file
			setenv("UPLOADED_FILE_PATH", tmpPath.c_str(), 1);
		}
	}
}

void	CGIHandler::runChildProcess(int *fd, char** argv)
{
	close(fd[1]);
	setEnvironment();
	extern char** environ;
	fileUpload();

	if (dup2(fd[0], STDIN_FILENO) == -1)
		std::runtime_error("Failed to redirect stdout");
	// THE JAD DO NOT TOUCH IT! THIS IS FOR DEBUGGING
	int filefd = open("dummy.html", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR, 0777);
	if (filefd < 0)
		throw std::runtime_error("Failed to open a file");
	if (dup2(filefd, STDOUT_FILENO) == -1)
		std::runtime_error("Failed to redirect stdout");
	if (dup2(filefd, STDERR_FILENO) == -1)
		std::runtime_error("Failed to redirect stderr");

	close(fd[0]);
	close(filefd);
	if (!check_access(argv[0]) || !check_access(argv[1]))
		throw std::runtime_error("Unauthorized");

	if (execve(argv[0], argv, environ) == -1) {
		std::cerr << "execve failed: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CGIHandler::runParentProcess(int *fd)
{
	int status = 0;
	
	close(fd[0]);
	if (!cgi_.body.empty())
		write(fd[1], cgi_.body.c_str(), cgi_.body.length());
	close(fd[1]);

	waitpid(-1, &status, 0);
	if (!WIFSIGNALED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = WTERMSIG(status) + 128;
	if (status != 0)
		throw std::runtime_error("execution problem");

	//setCGIResponse(tmpname);
}

void	CGIHandler::setCGIResponse(char* tmpname)
{
	std::fstream file(tmpname);
	std::string buffer;
	while(std::getline(file, buffer))
		cgiResponse_ += buffer + '\n';
}

std::string	CGIHandler::getCGIResponse()
{
	return cgiResponse_;
}

