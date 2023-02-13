#include "Socket.hpp"
using namespace webserv;

Socket::Socket(int domain, int service, int protocol, int port, \
	u_long interface) {
	/* define address structure */
	saddr_.sin_family = domain;
	saddr_.sin_port = htons(port);
	saddr_.sin_addr.s_addr = htonl(interface);

	/* establish socket */
	sfd_ = socket(domain, service, protocol);
	testConnection(sfd_);
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

int	Socket::getConnection(void) {
	return connection_;
}

void	Socket::setConnection(int connect) {
	connection_ = connect;
}

//virtual int establishConnection(int socket, struct sockaddr_in saddr)