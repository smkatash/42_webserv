#include "TestServer.hpp"
using namespace webserv;

TestServer::TestServer(): Server(AF_INET, SOCK_STREAM, 0, 4950, INADDR_ANY, 10) {
		launch();
}

void	TestServer::accepter() {
	struct sockaddr_in saddr = getSocket()->getSaddress();
	int	addrlen = sizeof(saddr);
	newsfd_ = accept(getSocket()->getSocket(), (struct sockaddr *)&saddr, (socklen_t*)&addrlen);
	numbytes_ = read(newsfd_, buff_, MAXSIZE);
	testTransfer(numbytes_, 0);
}

void	TestServer::handler() {
	std::cout << buff_ << std::endl;
}

void	TestServer::responder() {
	char resp[18] = "Hello from server";
	numbytes_ = write(newsfd_, resp, strlen(resp));
	testTransfer(numbytes_, strlen(resp));
	close(newsfd_);
}

void	TestServer::testTransfer(int bytes, ssize_t type) {
	/* error checking */
	if (type && type != bytes) {
		perror("Failed to write");
		exit(EXIT_FAILURE);
	} else if (type == 0 && bytes < 0 ) {
		perror("Failed to read");
		exit(EXIT_FAILURE);
	}
}

void	TestServer::launch() {
	for (;;) {
		std::cout << "server listening ..." << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "- successful session -" << std::endl;
	}
}

