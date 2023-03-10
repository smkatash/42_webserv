#include "Server.hpp"
using namespace webserv;

Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) {
	sockfd_ = new ListeningSocket(domain, service, protocol, port, interface, backlog);
}

ListeningSocket	*Server::getSocket() {
	return sockfd_;
}