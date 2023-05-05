#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserver.hpp"

class Server
{
	private: 
		int kqFd_;

		Socket serverSocket_;
		struct sockaddr_in serverAdd_;
		std::vector <Socket> clientsSocket_;
		
		void serverInit();

		ConfigFile config_;
		struct sockaddr_in initServerAddress(int port)
		Socket initServerSocket(int port, sockaddr_in serverAdd);
		std::vector <Socket> initClientsSocket()


	public:
		Server(ConfigFile conf, int kqFd);
		Server(int kqFd);
		~Server();


};