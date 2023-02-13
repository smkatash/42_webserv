#include "BindingSocket.hpp"
using namespace webserv;

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface): \
	Socket(domain,service, protocol, port, interface) {
	/* bind socket & error checking */
	setConnection(establishConnection(getSocket(), getSaddress()));
	testConnection(getConnection());
}

int BindingSocket::establishConnection(int socket, struct sockaddr_in saddr) {
	return bind(socket,(struct sockaddr*)&saddr, sizeof(saddr));
}
