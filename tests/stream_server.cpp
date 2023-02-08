#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
using namespace std;

#define PORT "3490"
#define BACKLOG 10


void sigchild_handler(int sig) {
	int current_errno = errno;
	while (waitpid(-1, NULL, WNOHANG) > 0);
	errno = current_errno;
}

void *getaddrinfo_helper(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
	int socket_fd, new_fd; // listen on socket fd, new connections on new_fd
	struct addrinfo hints, *result, *ptr;
	struct sockaddr_storage	clients;
	struct sigaction sa;
	socklen_t	sin_size;
	int	optval = 1;
	int	rvalue;
	char buff[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	/* get current address info, load linked list*/
	if ((rvalue = getaddrinfo(NULL, PORT, &hints, &result)) != 0) {
		cerr << "getaddrinfo: " << gai_strerror(rvalue) << endl;
		return 1;
	}

	/* loop through all the results and bind to the first we can */
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		/* create an endpoint for communication and returns a file 
		descriptor that refers to that endpoint.*/
		if ((socket_fd = socket(ptr->ai_family, \
			ptr->ai_socktype, ptr->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		/* to allocate buffer space, control timeouts, or permit 
		socket data broadcasts.*/

		/* SOL_SOCKET is the socket layer itself. It is used for options 
		that are protocol independent.
		SO_REUSEADDR Indicates that the rules used in validating addresses 
		supplied in a bind(2) call should allow reuse of local addresses.*/
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, \
			sizeof(int)) == -1) {
				perror("setsockopt");
				exit(1);
		}
		if (bind(socket_fd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			close(socket_fd);
			perror("server: bind");
			continue;
		}
		/* Successful bind, socket & ip connection established */
		break;
	}
	if (!ptr) {
		cerr << "server: failed to bind" << endl;
		exit(1);
	}
	freeaddrinfo(result);

	/* Listening ...*/
	if (listen(socket_fd, BACKLOG) == -1) {
		perror("listen failed");
		exit(1);
	}

	sa.sa_handler = sigchild_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction failed");
		exit(1);
	}
	cout << "server is listening ... " << endl;
	for (;;) {
		sin_size = sizeof(clients);
		new_fd = accept(socket_fd, (struct sockaddr *)&clients, &sin_size);
		if (new_fd == -1) {
			perror("accept failed");
			continue;
		}
		inet_ntop(clients.ss_family, \
			getaddrinfo_helper((struct sockaddr *)&clients), buff, sizeof(buff));
		cout << "Connection installed with: " << buff << endl;
		if (!fork()) {
			close(socket_fd);
			/* child process*/
			if (send(new_fd, "Hi there. This server!", 24, 0) == -1)
				perror("failed to send");
			close(new_fd);
			exit(0);
		}
		/* Parent close */
		close(new_fd); 
	}
	return 0;
}
