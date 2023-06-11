#include "Server.hpp"
#include "color.hpp"

Server::Server () : port_(6969) { }

Server::Server (ConfigFile config) : config_(config)
{
	port_ = config_.getListenPort();
}

Server::~Server () { }

//////////////////////////////////////////////////// set methods:
void Server::setServerAddress(struct sockaddr_in address) { serverAdd_ = address; }
void Server::setServerSocket(Socket socket) { serverSocket_ = socket; }
void Server::setClientsSocket(std::vector<Socket> sockets) { clientsSocket_ = sockets; }

//////////////////////////////////////////////////// get methods:
int Server:: getPort() { return this->port_; }
int Server::getServerSocketDescriptor()
{
	int serverSd;
	serverSd = serverSocket_.getSocketDescriptor();
	return (serverSd);
}
struct sockaddr_in Server::getServerAddress() { return (serverAdd_); }
Socket Server::getServerSocket() { return (serverSocket_); }
ConfigFile Server::getConfig() { return config_; }

//////////////////////////////////////////////////// set methods:
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

//////////////////////////////////////////////////// set methods:
bool Server::appendNewToSocketList(Socket socket)
{
	clientsSocket_.push_back(socket);
	return (true);
}

bool Server::serverInit()
{	
	struct sockaddr_in serverAdd;
	if (initServerAddress(&serverAdd) == false)
		return(printError("ERROR: server_address \t| SERVER port: ", port_ ));
	setServerAddress(serverAdd);
	Socket socket(getPort(), serverAdd_);
	if (initServerSocket(&socket) == false)
		return(printError("ERROR: setting ServerSocket \t| SERVER port: ", port_ ));
	setServerSocket(socket);
	return (true);
}