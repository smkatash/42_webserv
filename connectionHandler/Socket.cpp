
/*
	1) -Create a socket: The first step in creating a server is to create a socket that listens for incoming connections. 
		This can be achieved using the socket() function.

	2) -Bind the socket to an address: Once the socket is created, it needs to be bound to a specific address and port on the server.
		This can be achieved using the bind() function.

	3) -Listen for incoming connections: After binding the socket, the server needs to start listening for incoming connections. 
		This can be achieved using the listen() function.

	4) -Initialize the kqueue: The next step is to initialize the kqueue data structure, which will be used to monitor events on the socket.
		This can be achieved using the kqueue() function.

	5) -Add the socket to the kqueue: Once the kqueue is initialized, the server needs to add the socket to the kqueue
		This, using the kevent() function, will allow the server to monitor for incoming connections and data.
	
	6) -Monitor events using kqueue: The server can now monitor events using the kqueue data structure.
		This can be achieved using the kevent() function to check for new events on the socket.

	7) -Accept incoming connections: When a new connection is detected, the server can accept it using the accept() function.
		This will create a new socket for communication with the client.

	8) -Handle incoming data: Once a connection is established, the server can handle incoming data from the client. 
		This require recv() function.
		
	9) -Send data back to the client: Finally, the server can send data back to the client.
		This use the send() function.
*/

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