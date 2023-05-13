
#include "Core.hpp"


Core::Core (std::vector <ConfigFile> config, int kqFd) : kqFd_(kqFd)
{
	int i;
	std::vector <Server> servers;

	// for (i = 0; i < config.size(); i++) 
	// {
	// 	Server server(config[i], kqFd);
	// 	server.serverInit();
	// 	servers.push_back(server);
	// }
	Server server();
	server.serverInit();
	servers.push_back(server);
	this->servers_ = servers;
}
