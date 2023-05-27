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

int main(int argc, char **argv) {
	int	socket_fd;
	struct addrinfo hints, *result, *ptr;
	int rvalue;
	int	num_bytes;

	if (argc != 3) {
		std::cerr << "usage: client hostname message\n";
		exit(EXIT_FAILURE); 
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	rvalue = getaddrinfo(argv[1], PORT, &hints, &result);
	if (rvalue != 0) {
		std::cout << "getaddrinfo: " << gai_strerror(rvalue);
		return 1;
	}

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		socket_fd = socket(ptr->ai_family, ptr->ai_socktype, \
			ptr->ai_protocol);
		if (socket_fd == -1) {
			perror("socket failed");
			continue;
		}
		break;
	}

	if (ptr == nullptr) {
		std::cerr << "failed to create a scoket\n";
		return 2;
	}
	num_bytes = sendto(socket_fd, argv[2], strlen(argv[2]), 0, \
		ptr->ai_addr, ptr->ai_addrlen);
	if (num_bytes == -1) {
		perror("client: failed to send");
		exit(1);
	}

	freeaddrinfo(result);
	std::cout << "client: sent " << num_bytes << " to " << argv[1];
	close(socket_fd);
	return 0;
}
