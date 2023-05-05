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




void	CGIHandler::setEnvironment() {
	setenv("REQUEST_METHOD", req_.rline.method.c_str(), 1);
	setenv("CONTENT_TYPE", req_.eheader.contentType.c_str(), 1);
	setenv("CONTENT_LENGTH", req_.eheader.contentLength.c_str(), 1);
	setenv("HTTP_USER_AGENT", req_.rheader.userAgent.c_str(), 1);
	setenv("SCRIPT_NAME", scriptPath_.c_str(), 1);
	setenv("QUERY_STRING", queryString_.c_str(), 1);
	setenv("SERVER_NAME", serverName_.c_str(), 1);
}

char** transformString(std::string str1, std::string str2) {
	char **arr = (char**)malloc(sizeof(char*) * 3);
	char *arr[0] = malloc((sizeof(char) * str1.length()) )

	std::strcpy(char_array[0], str1);
	std::strcpy(char_array[1], str2);

	for (int i = 0; i < num; i++) {
		std::cout << char_array[i] << std::endl;
		delete[] char_array[i];
	}
}


void	CGIHandler::run() {
	pid_t pid = fork();

	if (pid == -1) {
		std::cerr << "Error forking process" << std::endl;
		exit(1);
	} else if (pid == 0) {
		
	setEnvironment();
	// Prepare the arguments for execve
	char *script = std::getenv("SCRIPT_NAME");
	char **argv = 
	extern char** environ;

	// Execute the script using execve
	if (execve(script_name, argv, envp) == -1) {
		// Error handling if execve fails
		std::cerr << "Error executing script" << std::endl;
		exit(1);
	}

		// Error handling if execl fails
		std::cerr << "Error executing script" << std::endl;
		exit(1);
	} else {
		// Parent process
		// Wait for the child process to finish
		int status;
		waitpid(pid, &status, 0);

		if (WIFEXITED(status)) {
			// Child process exited normally
			int exit_status = WEXITSTATUS(status);
			std::cout << "CGI script exited with status " << exit_status << std::endl;
		} else {
			// Child process exited abnormally
			std::cerr << "CGI script exited abnormally" << std::endl;
			exit(1);
		}
	}
}

void	CGIHandler::POST() {

}

void	CGIHandler::GET() {

}