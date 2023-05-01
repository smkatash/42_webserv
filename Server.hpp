#include "./connectionHandler/Socket.hpp"
#include "./parse/Parser.hpp"

class Server
{
	private: 
		Socket serverSocket_;
		int kqFd_;
		struct sockaddr_in serverAdd_;
		std::vector <Socket> sockets_;
		ConfigFile config_;

	public:
		Server(ConfigFile conf, int kqFd);
		~Server();
		
};