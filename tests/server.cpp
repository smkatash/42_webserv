#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <iostream>
#include <functional> 
#include <arpa/inet.h> 
#include <netinet/in.h>
using namespace std;

#define BUF_SIZE 500

int main(int argc, char *argv[]) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int socketFD, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (argc != 2) {
		cerr << "Usage: server port\n";
		exit(EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(NULL, argv[1], &hints, &result);
	if (s != 0) {
		cerr << "getaddrinfo: " << gai_strerror(s) << endl;
		exit(EXIT_FAILURE);
	}
		/* getaddrinfo() returns a list of address structures.
			Try each address until we successfully bind(2).
			If socket(2) (or bind(2)) fails, we (close the socket
			and) try the next address.
		*/

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		socketFD = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (socketFD == -1)
			continue;

		if (bind(socketFD, rp->ai_addr, rp->ai_addrlen) == 0) {
			cout << "Bind successfull !" << endl;
			break;                  /* Success */

		}

		close(socketFD);
	}
	if (rp == NULL) {               /* No address succeeded */
		cerr << "Could not bind\n";
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


	cout << "Starting to read ... " << endl;
	/* Read datagrams and echo them back to sender. */
	for (;;) {
		peer_addr_len = sizeof(peer_addr);
		nread = recvfrom(socketFD, buf, BUF_SIZE, 0,(struct sockaddr *) &peer_addr, &peer_addr_len);
		if (nread == -1)
				continue;               /* Ignore failed request */
		char host[NI_MAXHOST], service[NI_MAXSERV];
		s = getnameinfo((struct sockaddr *) &peer_addr,peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
		if (s == 0)
			cout << nread << " bytes received from " << host << ":" << service << " : " << buf << endl;
		else
			cerr << "getnameinfo: " << gai_strerror(s) << endl;
		strcpy(buf, "Thank you for message! This is server.");
		if (sendto(socketFD, buf, 40, 0, (struct sockaddr *) &peer_addr, peer_addr_len) == -1)
			cerr << "Error sending response\n";
		}
	cout << "Connection closed" << endl;
	return 0;
}