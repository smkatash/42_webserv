#include "cgiHandler.hpp"

CGIHandler::CGIHandler() {};

CGIHandler::~CGIHandler() {};

void CGIHandler::handle_cgi_script() {
	int pipefd[2];
	int status;

	if (pipe(pipefd) == -1) {
		throw("Failed to create pipe for CGI script");
	}
	pid_t pid = fork();
	if (pid == -1) {
		throw("Failed to fork process for CGI script");
	}

	// Child process
	if (pid == 0) {
		// Set up environment variables for CGI script
		setenv("REQUEST_METHOD", req_.rline.method.c_str(), 1);
		setenv("QUERY_STRING", queryString_.c_str(), 1);
		setenv("CONTENT_TYPE", req_.eheader.contentType.c_str(), 1);
		setenv("CONTENT_LENGTH", req_.eheader.contentLength.c_str(), 1);

		// Redirect standard output to pipe
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		
		extern char **environ;
		// TODO Execute CGI script -fix arg path
		const char *args = scriptPath_.c_str();
		// execve(scriptPath_.c_str(), scriptPath_.c_str()(char **)environ);
		std::cerr << "Failed to execute CGI script" << std::endl;
		exit(EXIT_FAILURE);
	} else {
		// Send inputBody_ to child process
		close(pipefd[1]);
		write(pipefd[0], inputBody_.c_str(), inputBody_.length());
		close(pipefd[0]);

		waitpid(pid, &status, 0);

		// Read output from child process
		std::string output;
		char buf[BUFSIZ];
		ssize_t nread;
		while ((nread = read(pipefd[0], buf, BUFSIZ)) > 0) {
			output.append(buf, nread);
		}

		// Send output back to client
		std::cout << "Content-type: text/html\r\n\r\n";
		std::cout << output;
	}
}