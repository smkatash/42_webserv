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
using namespace std;

#define PORT "4950"
#define BUFF_SIZE 100

int main(void) {
	int socket_fd;
	struct addrinfo hints, *result, *ptr;
	struct sockaddr_storage clients;
	socklen_t	addrlen;
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
		cerr << "getaddrinfo: " << gai_strerror(rvalue) << endl;
		return 1;
	}

	/* Loop through the linked list to get */
}
