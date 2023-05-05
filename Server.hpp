#include "./connectionHandler/Socket.hpp"
#include "./parse/Parser.hpp"

class Server
{
	private: 
		int kqFd_;
		Socket serverSocket_;
		struct sockaddr_in serverAdd_;
		std::vector <Socket> clientsSocket_;
		
		void serverInit();

		ConfigFile config_;

	public:
		Server(ConfigFile conf, int kqFd);
		~Server();

};