#include "Socket.hpp"
using namespace webserv;

Socket::Socket(int domain, int service, int protocol, int port, \
	u_long interface) {
	/* define address structure */
	saddr_.sin_family = domain;
	saddr_.sin_port = htons(port);
	saddr_.sin_addr.s_addr = inet_addr("192.168.51.68");
	/* establish socket */
	sfd_ = socket(domain, service, protocol);
	testConnection(sfd_);
	setSocketOpt();
}

void Socket::setSocketOpt() {
	int	optval;
	/* allow sock reuse*/
	if (setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, &optval,\
			sizeof(optval)) == -1) {
				perror("setsockopt");
				exit(1);
	}
}

void Socket::testConnection(int fd) {
	/* error checking */
	if (fd < 0) {
		perror("Failed to connect");
		exit(EXIT_FAILURE);
	}
}

int	Socket::getSocket(void) {
	return sfd_;
}

struct sockaddr_in	Socket::getSaddress(void) {
	return saddr_;
}

//virtual int establishConnection(int socket, struct sockaddr_in saddr)