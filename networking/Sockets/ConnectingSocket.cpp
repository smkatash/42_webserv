#include "ConnectingSocket.hpp"
using namespace webserv;

ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface): \
	Socket(domain,service, protocol, port, interface) {
	/* bind socket & error checking */
	setConnection(establishConnection(getSocket(), getSaddress()));
	testConnection(getConnection());
}

int ConnectingSocket::establishConnection(int socket, struct sockaddr_in saddr) {
	return connect(socket,(struct sockaddr*)&saddr, sizeof(saddr));
}
