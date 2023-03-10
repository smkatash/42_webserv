#include "ConnectingSocket.hpp"
using namespace webserv;

ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : \
	Socket(domain,service, protocol, port, interface) {
	/* bind socket & error checking */
	establishConnection(getSocket(), getSaddress());
	testConnection(connect_);
}

void ConnectingSocket::establishConnection(int socket, struct sockaddr_in saddr) {
	connect_ = connect(socket,(struct sockaddr*)&saddr, sizeof(saddr));
}
