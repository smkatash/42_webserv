#include "../inc/Server.hpp"

//INIT------------------------------------------------------------------------
Server::
Server (ConfigFile conf) : config_(conf)
{
	std::vector<Socket> clientsSocket = {};
	clientsSocket_ = clientsSocket;
}

//SET-------------------------------------------------------------------------
void Server::setServerAddress(struct sockaddr_in address)
{
	serverAddress_ = address;
}

void Server::setServerSocket(std::vector<Socket> socket)
{
	serverSocket_ = socket;
}

void Server::setClientSocket(std::vector<Socket> socket)
{
	clientSocket_ = socket;
}
//GET-------------------------------------------------------------------------
int Server:: getPort()
{
	// with config_file:
	// return (this->config_.getListenPort());
	return this->port_
}

int Server::getServerFd()
{
	int serverFd;
	serverFd = this->serverSocket.getServerSd();
	return (serverFd);
}

// INIT-----------------------------------------------------------------------
void Socket::initServerAddress()
{
	struct sockaddr_in serverAdd = {};

	memset(&(serverAdd), 0, sizeof(serverAdd));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAdd.sin_port = htons(getPort());
	setServerAddress(serverAdd);
}

Socket Server::initServerSocket(int port, sockaddr_in serverAdd, int kq)
{
	Socket serverSocket(port, serverAdd, kq);
	if(serverSocket.socketPassiveInit() == false);
		return (NULL);
	setSocket(serverSocket);
	return serverSocket;
}

std::vector <Socket> Server::initClientsSocket()
{
	std::vector <Socket> clientSockets;
	return (clientSockets);
}

//MAIN------------------------------------------------------------------------
bool Server::addClientInSockets()
{
	Socket()
}


void Server::
serverInit()
{	
	// with configuration file:
		// this->serverAdd_ = this->initServerAddress(server.config_.getListenPort());
		// this->serverSocket_ = this->initServerSocket(server.config_.getListenPort(), server->serverAdd);
	serverAdd_ = initServerAddress(getListenPort());
	serverSocket_ = initServerSocket(getListenPort(), serverAdd);	
}