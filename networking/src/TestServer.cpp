#include "TestServer.hpp"
using namespace webserv;

TestServer::TestServer(): Server(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY, 10) {
		launch();
}

void	TestServer::accepter() {
	struct sockaddr_in saddr = getSocket()->getSaddress();
	int	addrlen = sizeof(saddr);
	newsfd_ = accept(getSocket()->getSocket(), (struct sockaddr *)&saddr, (socklen_t*)&addrlen);
	char ipstr[INET6_ADDRSTRLEN];
	std::cout << "---------------------------------------" << std::endl;
	inet_ntop(saddr.sin_family, (struct sockaddr *)&saddr.sin_addr, ipstr, sizeof(ipstr));
		std::cout << "Connection installed with: " << ipstr << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	numbytes_ = read(newsfd_, buff_, MAXSIZE);
	testTransfer(numbytes_, 0);
}

void	TestServer::handler() {
	std::cout << "Reading: " << buff_ << std::endl;
}

void	TestServer::responder() {
	// char resp[18] = "Hello from server";
	char arr[]= "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 14\n\n<h1>You bitch!</h1>";
	numbytes_ = send(newsfd_, arr, sizeof(arr), 0);
	//numbytes_ = write(newsfd_, arr, strlen(arr));
	testTransfer(numbytes_, strlen(arr) + 1);
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
	int counter = 0;
	for (;;) {
		std::cout << "server listening ..." << std::endl;
		std::cout << "count is " << counter << std::endl;
		accepter();
		handler();
		responder();
		counter++;
		std::cout << "- successful session -" << std::endl;
	}
}

