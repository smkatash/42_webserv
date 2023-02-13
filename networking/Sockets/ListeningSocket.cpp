#include "ListeningSocket.hpp"
using namespace webserv;

ListeningSocket::ListeningSocket(int domain, int service, int protocol, \
								int port, u_long interface, int backlog): \
BindingSocket(domain, service, protocol, port, interface) {
	/* backlog to wait */
	backlog_ = backlog;
	startListening();
	testConnection(listenfd_);
}


void	ListeningSocket::startListening() {
	listenfd_ = listen(getConnection(), backlog_);
}