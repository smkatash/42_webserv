#include "../inc/Socket.hpp"

Socket::Socket(int port, struct sockaddr_in servAdd): port_(port), serverAddress_(servAdd) 
{
}

Socket::~Socket(){}

bool Socket::setSocketDescriptor()
{
	int fd;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return false;
	this->sD_ = fd;
	return (true);
}

bool Socket::setSocketOption()
{
	// set the SOCKET as TCP_NODELAY, for fast response whenever possible.
	int val = 1;
	if( setsockopt(this->Sd_, 0, TCP_NODELAY, (char *)&val, sizeof(int)) == 0)
		return (false);
	return (true);
}

void Socket::setServerAddress()
{
	//should work with an IP but right now it takes a random one.;
	memset(&(this->serverAddress_), 0, sizeof(this->serverAddress_));
	this->serverAddress_.sin_family = AF_INET;
	this->serverAddress_.sin_addr.s_addr = INADDR_ANY;
    this->serverAddress_.sin_port = htons(this->getPort());
}

struct kevent Socket::getEvent()
{
	return (this->events_);
}

bool Socket::setKevent()
{
	EV_SET(&(this->getEvent()[0]), kqFd, EVFILT_READ, EV_ADD, 0, 0, socket);
	EV_SET(&(this->getEvent()[1]), kqFd, EVFILT_WRITE, EV_ADD, 0, 0, socket);
	if (kevent(kqFd, &(this->getEvent()), 2, NULL, 0, NULL) == 0)
		return false;
	return true;
}

int Socket::getPort()
{
	return (this->port_);
}

int Socket::getSocketDescriptor()
{
	return (this->Sd_);
}

struct sockaddr_in Socket::getAddress(int n)
{
	if (n == 0)
		return (this->serverAddress_);
	else 
		return (this->clientAddress_);
}

bool Socket::setSocketBind()
{
	int retValue;
	socklen_t addrLen;

	addrLen = sizeof(this->getAddress(0));
	retValue = bind(this->getSocketDescriptor(), this->getAddress(), addrLen);

	return( retValue == 0 ? true : false);
}

bool Socket::setSocketPassive()
{
	int retValue;
	retValue = listen(this->getSocketDescriptor(), 5);
	return(retValue == 0 ? true : false);
}

bool Socket::socketInit()
{
	// init the socket;
	// #1
	if (this->setSocketDescriptor() == false);
		printf("socket() error \n");
	if (this->setSocketOption() == false);
		printf("setsockopt() error \n");
	// socket->setServerAddress();
	// #2
	if(this->socketBind() == false)
		printf("bind_error \n");
	// #3
	if(this->setSocketPassive(&socket) == false);
		printf("listen_error \n");
	// #5
	// add the socket to the kqueue;
	if(socket->setKvent() == false)
	return true;
}