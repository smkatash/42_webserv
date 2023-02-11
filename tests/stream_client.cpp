#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;
#define PORT "9034"
#define MAXDATASIZE 100

void *getaddrinfo_helper(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv) {
	int	socket_fd, numbytes;
	char message[MAXDATASIZE];
	struct addrinfo hints, *result, *ptr;
	int rvalue;
	char	buff[INET6_ADDRSTRLEN];

	if (argc != 2) {
		cerr << "usage: ./client hostname\n";
		exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rvalue = getaddrinfo(argv[1], PORT, &hints, &result)) != 0) {
		cerr << "getaddrinfo: " << gai_strerror(rvalue);
		return 1;
	}

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		if ((socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol) == -1)) {
			perror("client: socket failed");
			continue;
		}
		int rv = connect(socket_fd, ptr->ai_addr, ptr->ai_addrlen);

		if (rv == -1) {
			close(socket_fd);
			perror("client: connect failed");
			continue;
		}
		break;
	}

	if (ptr == nullptr) {
		cerr << "client: failed to connect\n";
		return 2;
	}

	inet_ntop(ptr->ai_family, \
			getaddrinfo_helper((struct sockaddr *)ptr->ai_addr), buff, sizeof(buff));
	cout << "Connection installed with: " << buff << endl;
	freeaddrinfo(result);

	if ((numbytes = recv(socket_fd, message, MAXDATASIZE - 1, 0)) == -1) {
		perror("client: failed to receive");
		exit(1);
	}
	message[numbytes] = '\0';
	cout << "Received: " << message << endl;
	close(socket_fd);
	return 0;
}