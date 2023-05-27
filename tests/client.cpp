#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <functional> 
#include <arpa/inet.h> 
#include <netinet/in.h>
	
	using namespace std;

	#define BUF_SIZE 500

int main(int argc, char *argv[]) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (argc < 3) {
		cerr << "Usage: host port msg...\n";
		exit(EXIT_FAILURE);
	}

	/* Obtain address(es) matching host/port. */

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (s != 0) {
	cerr << "getaddrinfo: " << gai_strerror(s) << endl;
	exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully connect(2).
	If socket(2) (or connect(2)) fails, we (close the socket
	and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;
	if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
		cout << "Successfully connected!";
		break;                  /* Success */
	}
	close(sfd);
	}
	if (rp == NULL) {               /* No address succeeded */
	cerr << "Could not connect\n";
	exit(EXIT_FAILURE);
	}
	char ipstr[INET6_ADDRSTRLEN];
	void *addr;
	std::string ipver;
	if (rp->ai_family == AF_INET) { // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr; 
		addr = &(ipv4->sin_addr);
		ipver = "IPv4";
	} else { // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr; 
		addr = &(ipv6->sin6_addr);
		ipver = "IPv6";
	}
	inet_ntop(rp->ai_family, addr, ipstr, sizeof ipstr);
	cout << "Current IP: " << ipver << " " << ipstr << endl;
	freeaddrinfo(result);           /* No longer needed */

	/* Send remaining command-line arguments as separate
	datagrams, and read responses from server. */

	for (int j = 3; j < argc; j++) {
			len = strlen(argv[j]) + 1;
			/* +1 for terminating null byte */

	if (len > BUF_SIZE) {
		cerr << "Ignoring long message in argument "<< argv[j];
		continue;
	}

	if (write(sfd, argv[j], len) != len) {
		cerr << "partial/failed write\n";
		exit(EXIT_FAILURE);
	}

	nread = read(sfd, buf, BUF_SIZE);
	if (nread == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	cout << nread << " bytes received: " << buf;
	}
	exit(EXIT_SUCCESS);
}