#include "Server.hpp"

Server::Server ()
: port_(8080)
{
}

Server::~Server ()
{
}

//SET-------------------------------------------------------------------------
void Server::setServerAddress(struct sockaddr_in address)
{
	serverAdd_ = address;
}

void Server::setServerSocket(Socket socket)
{
	serverSocket_ = socket;
}

void Server::setClientsSocket(std::vector<Socket> sockets)
{
	clientsSocket_ = sockets;
}

//GET-------------------------------------------------------------------------
int Server:: getPort()
{
	return this->port_;
}

int Server::getServerSocketDescriptor()
{
	int serverSd;
	serverSd = serverSocket_.getSocketDescriptor();
	return (serverSd);
}

struct sockaddr_in Server::getServerAddress()
{
	return (serverAdd_);
}

// INIT-----------------------------------------------------------------------
bool Server::initServerAddress(struct sockaddr_in* serverAdd)
{
	memset(serverAdd, 0, sizeof(*serverAdd));
	serverAdd->sin_family = AF_INET;
	serverAdd->sin_addr.s_addr = htonl(INADDR_ANY);
	serverAdd->sin_port = htons(getPort());
	return (true);
}

bool Server::initServerSocket(Socket *socket)
{
	if (socket->socketPassiveInit() == false)
		return (false);
	return (true);
}

//MAIN------------------------------------------------------------------------
bool Server::appendNewToSocketList(Socket socket)
{
	clientsSocket_.push_back(socket);
	return (true);
}

bool Server::serverInit()
{	
	struct sockaddr_in serverAdd;
	if (initServerAddress(&serverAdd) == false)
	{
		printf("Error setting address");
		return (false);
	}
	setServerAddress(serverAdd);
	Socket socket(getPort(), serverAdd_);
	if (initServerSocket(&socket) == false)
	{
		printf("Error setting ServerSocket");
		return (false);
	}
	setServerSocket(socket);
	return (true);
}