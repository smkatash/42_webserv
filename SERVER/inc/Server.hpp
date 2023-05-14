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
		void setServerAddress(struct sockaddr_in address);
		void setServerSocket(Socket socket);
		void setClientsSocket(std::vector<Socket> socket);


		int getPort();
		int getServerFd();
		struct sockaddr_in getServerAddress();

		std::vector <Socket> initClientsSocket();
		bool initServerAddress(struct sockaddr_in* serverAdd);
		bool initServerSocket(Socket *socket);
		// main
		bool serverInit();
		bool appendNewToSocketList(Socket socket);

};

#endif