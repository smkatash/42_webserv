#include "../inc/Server.hpp"

//INIT------------------------------------------------------------------------
Server::
Server (ConfigFile conf) : config_(conf)
{
}

//SET-------------------------------------------------------------------------

struct sockaddr_in Server::
initServerAddress(int port)
{
	struct sockaddr_in serverAdd;
	serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons(port);
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	return serverAdd;
}

//GET-------------------------------------------------------------------------
int Server:: 
getPort()
{
	// with config_file:
	// return (this->config_.getListenPort());
	return this->port_
}
int Server::
getServerFd()
{
	int serverFd;
	serverFd = this->serverSocket.getServerSd();
	return (serverFd);
}

// INIT-----------------------------------------------------------------------
void Socket::
initServerAddress()
{
	memset(&(this->serverAddress_), 0, sizeof(this->serverAddress_));
	this->serverAddress_.sin_family = AF_INET;
	this->serverAddress_.sin_addr.s_addr = INADDR_ANY;
	this->serverAddress_.sin_port = htons(this->getPort());
}

Socket Server::
initServerSocket(int port, sockaddr_in serverAdd, int kq)
{
	Socket serverSocket(port, serverAdd, kq);
	if(serverSocket.socketInit() == false);
		return (NULL);
	return serverSocket;
}

std::vector <Socket> Server::
initClientsSocket()
{
	std::vector <Socket> clientSockets;
	return (clientSockets);
}

//MAIN------------------------------------------------------------------------
void Server::
serverInit()
{	
	this->clientsSocket_ = this->initClientsSocket();
	// with configuration file:
		// this->serverAdd_ = this->initServerAddress(server.config_.getListenPort());
		// this->serverSocket_ = this->initServerSocket(server.config_.getListenPort(), server->serverAdd);
	this->serverAdd_ = this->initServerAddress(this->getListenPort());
	this->serverSocket_ = this->initServerSocket(this->getListenPort(), this->serverAdd);	
}