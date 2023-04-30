#include "Server.hpp"
#include "./connectionHandler/Socket.hpp"
Server::Server(ConfigFile conf) : config_(conf)
{
	int i;
	// for ( i = 0; i < conf.maxConnection; i++)
	for (i = 0; i < 10; i++)
	{
		socket(int port, int ip);
		
	}
}