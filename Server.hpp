#include "./connectionHandler/Socket.hpp"
#include "./parse/Parser.hpp"

class Server
{
	private: 
		std::vector <Socket> sockets_;
		ConfigFile config_;

	public:
		Server(ConfigFile conf);
		~Server();
		
};