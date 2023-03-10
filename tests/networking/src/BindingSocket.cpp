#include "BindingSocket.hpp"
using namespace webserv;

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface): \
	Socket(domain,service, protocol, port, interface) {
	/* bind socket & error checking */
	establishConnection(getSocket(), getSaddress());
	testConnection(binding_);
}

void BindingSocket::establishConnection(int socket, struct sockaddr_in saddr) {
	binding_ = bind(socket,(struct sockaddr*)&saddr, sizeof(saddr));
}
