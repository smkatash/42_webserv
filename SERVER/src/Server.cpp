#include "../inc/Server.hpp"

//INIT------------------------------------------------------------------------
Server::Server (ConfigFile conf, int kqFd) : config_(conf), kqFd_(kqFd)
{
}

void Socket::setServerAddress()
{
	memset(&(this->serverAddress_), 0, sizeof(this->serverAddress_));
	this->serverAddress_.sin_family = AF_INET;
	this->serverAddress_.sin_addr.s_addr = INADDR_ANY;
	this->serverAddress_.sin_port = htons(this->getPort());
}

//SET-------------------------------------------------------------------------
Socket 
setServerSocket(int port, sockaddr_in serverAdd)
{
	Socket serverSocket(port, serverAdd);
}

struct sockaddr_in 
setServerAddress(int port)
{
	struct sockaddr_in serverAdd;
	serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons(port);
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	return serverAdd;
}
//GET-------------------------------------------------------------------------
int 
getPort()
{
	return (this->config_.getListenPort());
}

std::vector <Socket> 
initClientsSocket(){
	std::vector <Socket> clientSockets;
	return (clientSockets);
}

//MAIN------------------------------------------------------------------------
void Server::
serverInit()
{
	this->clientsSocket_ = initClientsSocket();
	server->serverAdd_ = setServerAddress(server.config_.getListenPort());
	server->serverSd_ = setServerSocket(server.config_getListenPort(), server->serverAdd);
}