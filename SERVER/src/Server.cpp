#include "../inc/Server.hpp"

//INIT------------------------------------------------------------------------
Server::Server (ConfigFile conf) : config_(conf)
{
	port_ = 8080;
	std::vector<Socket> clientsSocket = {};
	clientsSocket_ = clientsSocket;
}
Server::Server ()
{
	ConfigFile config;
	config.setListenPort(80);
	config.setServerName("test");
	config.setClientMaxBodySize(10);

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

void Server::setClientsSocket(std::vector<Socket> socket)
{
	clientsSocket_ = socket;
}
//GET-------------------------------------------------------------------------
int Server:: getPort()
{
	// with config_file:
	// return (this->config_.getListenPort());
	return this->port_;
}

int Server::getServerFd()
{
	int serverFd;
	serverFd = serverSocket_.getSocketDescriptor();
	return (serverFd);
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
	if(socket->socketPassiveInit() == false)
		return (false);
	return (true);
}

std::vector <Socket> Server::initClientsSocket()
{
	std::vector <Socket> clientSockets;
	return (clientSockets);
}   

//MAIN------------------------------------------------------------------------
bool Server::appendNewToSocketList(Socket socket)
{
	clientsSocket_.push_back(socket);
	return (true);
}

bool Server::serverInit()
{	
	struct sockaddr_in serverAdd = {}; //new address struct;
	// with configuration file:
		// this->serverAdd_ = this->initServerAddress(server.config_.getListenPort());
		// this->serverSocket_ = this->initServerSocket(server.config_.getListenPort(), server->serverAdd);
	if(initServerAddress(&serverAdd) == false)
	{
		printf("Error setting address");
		return (false);
	}
	setServerAddress(serverAdd);
	Socket socket(getPort(), serverAdd_);
	if(initServerSocket(&socket) == false)
	{
		printf("Error setting ServerSocket");
		return (false);
	}
	setServerSocket(socket);
	return (true);
}