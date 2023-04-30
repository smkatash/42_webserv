
#include "Core.hpp"


Core::Core (std::vector <ConfigFile> config)
{
	int i;
	std::vector <Server> servers;

	for (i = 0; i < config.size(); i++) 
	{
		Server server(config[i]);
		servers.push_back(server); 
	}
	this->servers_ = servers;
}