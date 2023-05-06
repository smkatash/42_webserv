#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserver.hpp"

class Server
{
	private: 
		Socket serverSocket_;
		struct sockaddr_in serverAdd_;
		std::vector <Socket> clientsSocket_;
		int port_ = 8080;

		// ConfigFile config_;

	public:
		Server(ConfigFile conf);
		Server();
		~Server();

		int getPort();
		int getServerFd();
		bool serverInit();
		struct sockaddr_in initServerAddress(int port)
		Socket initServerSocket(int port, sockaddr_in serverAdd);
		std::vector <Socket> initClientsSocket();

};