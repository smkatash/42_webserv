#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Socket.hpp"

class Server
{
	private: 
		Socket serverSocket_;
		struct sockaddr_in serverAdd_;

		std::vector <Socket> clientsSocket_;
		int port_;

		// ConfigFile config_;

	public:
		// Server(ConfigFile conf);
		Server();
		~Server();

		void setServerAddress(struct sockaddr_in address);

		int getPort();
		int getServerFd();
		struct sockaddr_in getServerAddress();


		bool serverInit();
		struct sockaddr_in initServerAddress(int port);
		Socket initServerSocket(int port, sockaddr_in serverAdd);
		std::vector <Socket> addClientInSockets();
};

#endif