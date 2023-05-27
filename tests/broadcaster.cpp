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

#define PORT 4950

int main(int argc, char **argv) {
	int	socket_fd;
	struct sockaddr_in clients;
	struct hostent *he;
	int	num_bytes;
	int broadcast = 1;

	if (argc != 3) {
		std::cerr << "usage: broadcaster hostname message\n";
		exit(EXIT_FAILURE); 
	}

	he = gethostbyname(argv[1]);
	if (!he) {
		perror("hostname not found");
		exit(1);
	}

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		perror("failed to get the socket");
		exit(1);
	}
	/* allows broadcast packets to be sent */
	if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1) {
		perror("setsockopt failed");
		exit(1);
	}

	clients.sin_family = AF_INET;
	clients.sin_port = htons(PORT);
	clients.sin_addr = *((struct in_addr *)he->h_addr);
	memset(clients.sin_zero, '\0', sizeof(clients.sin_zero));

	num_bytes = sendto(socket_fd, argv[2], strlen(argv[2]), 0, (struct sockaddr *)&clients, sizeof(clients));
	if (num_bytes == -1) {
		perror("failed to send");
		exit(1);
	}
	std::cout << "sent: " << num_bytes << " to " << inet_ntoa(clients.sin_addr) << std::endl;
	close(socket_fd);
	return 0;
}
