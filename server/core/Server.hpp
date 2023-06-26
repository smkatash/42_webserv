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
		Server();
		~Server();
		Server (ConfigFile config);

		void setServerAddress(struct sockaddr_in address);
		void setServerSocket(Socket socket);
		void setClientsSocket(std::vector<Socket> socket);
		
		int getPort();
		int getServerSocketDescriptor();
		Socket getServerSocket();
		ConfigFile getConfig();
		struct sockaddr_in getServerAddress();

		std::vector <Socket> initClientsSocket();
		bool initServerAddress(struct sockaddr_in* serverAdd);
		bool initServerSocket(Socket *socket);
		bool serverInit();
		bool appendNewToSocketList(Socket socket);
};

#endif