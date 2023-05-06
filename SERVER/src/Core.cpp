#include "../inc/Core.hpp"

Core::Core ()
{
	Server server();
	if(server.serverInit())
		serverClose();
	this->server_ = server;

	// void eventWrite(Server server);
	// void eventRead(Server server);
	// this->servers_ = servers;
}

void Core::Run()
{
	while(1)
	{
		
	}
}