#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/event.h>
#include <stdint.h>
#include <machine/types.h>
#include <vector>
#include "ConfigFile.hpp"


class Server
{
	private: 
		Socket serverSocket_;
		struct sockaddr_in serverAdd_;

		std::vector <Socket> clientsSocket_;
		int port_;
		ConfigFile config_;
		int timeoutInSecond_;

	public:
		// Server(ConfigFile conf);
		Server();
		~Server();
		Server (ConfigFile config);

		// SET----------------------------------------------------
		void setServerAddress(struct sockaddr_in address);
		void setServerSocket(Socket socket);
		void setClientsSocket(std::vector<Socket> socket);
		// GET----------------------------------------------------
		int getPort();
		int getServerSocketDescriptor();
		struct sockaddr_in getServerAddress();
		Socket getServerSocket();

		// INIT---------------------------------------------------
		std::vector <Socket> initClientsSocket();
		bool initServerAddress(struct sockaddr_in* serverAdd);
		bool initServerSocket(Socket *socket);
		// main---------------------------------------------------
		bool serverInit();
		bool appendNewToSocketList(Socket socket);

};

#endif