
#include "../inc/Core.hpp"

Core::Core (int kqFd) : kqFd_(kqFd)
{
	int i;
	
	Server server(kqFd);
	server.serverInit();
	this->server_ = server;

	// this->servers_ = servers;
}

void Core::Run()
{

	while(1)
	{
		
	}
}