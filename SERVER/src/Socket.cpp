#include "Socket.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

Socket::Socket(int port, int kqFd): port_(port), kqFd_(kqFd); 
{
}

Socket::~Socket(){}

bool Socket::setServerSd()
{
	int fd;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return false;
	this->serverSd_ = fd;
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

int Socket::getKqueue()
{
	return (this->kqFd_);
}

struct kevent Socket::getEvent()
{
	return (this->event_);
}


bool setKEvent(Socket *socket)
{
	EV_SET(&(this->getEvent()), this->getServerSd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	this->getEvent() = kevent(this->getKqueue(), &(this->getEvent()), 1, NULL, 0, NULL);
	if (this->nEvent_ == -1)
		return (false);
}

int Socket::getPort()
{
	return (this->port_);
}

int Socket::getServerSd()
{
	return (this->serverSd_);
}



struct sockaddr_in Socket::getAddress(int n)
{
	if (n == 0)
		return (this->serverAddress_);
	else 
		return (this->clientAddress_);
}
/*
	1#	A process creates a socket by calling the socket() system call. 
		This creates a SOCKET DESCRIPTOR, which is used to identify the socket in subsequent system calls.

	2#	The process then binds the socket to a specific address and port on the local machine by calling the bind() system call.
		This step is optional, as the operating system can assign an available port automatically if one is not specified.

	3#	The process then listens on the socket for incoming connections by calling the listen() system call.

	4#  When a remote process wants to establish a connection, it first creates its own socket and then connects to the local socket by calling the connect() system call.
		The local process can also initiate a connection by calling the connect() system call on its own socket.

	Once a connection is established, data can be exchanged between the two processes by calling the send() and recv() system calls.
	When the communication is complete, the processes close their sockets by calling the close() system call.
*/

bool socketInit(Socket *socket)
{
	// #1
	if (socket->setServerSd() == false);
		printf("bind_error \n");
	socket->setServerAddress();
	// #2
	if(socketBind(socket) == false)
		printf("bind_error \n");
	// #3
	if(setSocketPassive(socket) == false);
		printf("listen_error \n");
	// #5
	if(socket->setKqueue())
	return true;
}

bool socketBind(Socket *socket)
{
	int retValue;
	socklen_t addrLen;

	addrLen = sizeof(socket->getAddress(0));
	retValue = (socket->getServerSd(), socket->getAddress(), addrLen);

	return( retValue == 0 ? true : false);
}

bool setSocketPassive(Socket *socket)
{
	int retValue;
	retValue = listen(socket->getServerSd(), 5);
	return(retValue == 0 ? true : false);
}