#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

#define PORT "4950"
#define BUFF_SIZE 100

void	*get_addrin(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
	int socket_fd;
	struct addrinfo hints, *result, *ptr;
	struct sockaddr_storage clients;
	socklen_t	addrlen;
	int		receivedbytes;
	char	message[BUFF_SIZE];
	char	ip[INET6_ADDRSTRLEN];
	int		rvalue;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	/* Get info to store about IP*/
	rvalue = getaddrinfo(NULL, PORT, &hints, &result);
	if (rvalue != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(rvalue) << std::endl;
		return 1;
	}

	/* Loop through the linked list to get */
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		if ((socket_fd = socket(ptr->ai_family, ptr->ai_socktype, \
			ptr->ai_protocol)) == -1) {
				perror("listener: socket failed");
				continue;
			}
		if (bind(socket_fd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			close(socket_fd);
			perror("listener: bind failed");
			continue;
		}
		break;
	}

	if (ptr == nullptr) {
		std::cerr << "listener: failed to bind socket.\n";
		return 2;
	}

	freeaddrinfo(result);
	std::cout << "listener: waiting to receive ...\n";

	addrlen = sizeof(clients);
	if ((receivedbytes = recvfrom(socket_fd, message, \
		BUFF_SIZE - 1, 0, (struct sockaddr *)&clients, &addrlen)) == -1) {
			perror("recvfrom failed");
			exit(1);
	}
	inet_ntop(clients.ss_family, get_addrin((struct sockaddr*)&clients), ip, sizeof(ip));
	std::cout << "listener got a packet from " << ip << std::endl;
	message[receivedbytes] = '\0';
	std::cout << "package contains: " << receivedbytes << " bytes, " << message << std::endl;
	close(socket_fd);
	return (0);
}
