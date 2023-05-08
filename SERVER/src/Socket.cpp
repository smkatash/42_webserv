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
	int val;
	int socketDescriptor;
	
	socketDescriptor = getSocketDescriptor();
	val = 1;
	if( setsockopt(socketDescriptor, 0, TCP_NODELAY, (char *)&val, sizeof(int)) == -1)
		return (false);
	return (true);
}

struct kevent Socket::getEvent()
{
	return (this->events_);
}

bool Socket::setKevent()
{
	EV_SET(&(this->getEvent()[0]), kqFd, EVFILT_READ, EV_ADD, 0, 0, socket);
	EV_SET(&(this->getEvent()[1]), kqFd, EVFILT_WRITE, EV_ADD, 0, 0, socket);
	if (kevent(kqFd, &(this->getEvent()), 2, NULL, 0, NULL) == -1)
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

vector<char> Socket::getData()
{
	return data_;
}

// SET---------------------------------------------------------
void Socket::setAddress ()
{
	struct sockaddr_in address = {};
    address.sin_family = AF_INET;
	// address.sin_addr.s_addr = htonl(getIp());
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(getPort());
	this->address_ = address;
	return (); 
}

struct sockaddr_in Socket::getSocketAddress()
{
	return (this->clientAddress_);
}

bool Socket::setSocketBind()
{
	int retValue;
	socklen_t addrLen;

	addrLen = sizeof(this->getAddress());
	retValue = bind(this->getSocketDescriptor(), this->getAddress(), addrLen);

	return( retValue == 0 ? true : false);
}

bool Socket::setSocketPassive()
{
	int retValue;
	retValue = listen(this->getSocketDescriptor(), 5);
	return(retValue == 0 ? true : false);
}

// something to fix here;
bool Socket::setData (vector<char> buffer) 
{
	data_ = buffer;
		return true;
	if ( buffer.size() == 0)
	{
		return false;
	}
}

bool Socket::readHandler()
{
	vector<char> buffer(5000);
	int socketDescriptor;
	int bytes;

	socketDescriptor = getSocketDescriptor();
	bytes = recv(socketDescriptor, buffer.data(), buffer.size(), 0);
	if( bytes > 0 )
	{
		// do something... 
		std::cout<< buffer.data << std::endl;
		setData(buffer);
		return buffer;
	} 
	else if ( bytes = 0) 
	{ 
		// server has finished sending data;
		// what else to do?
	}
	else if ( bytes < 0 && errno == EAGAIN)
	{
		// the socket read buffer is empty;
		// bytes is < 0 we have to return error;
	}
	else
	{
		// we have a problem;
	}
	return vector<char> buffer;
}

// we should sett an offset;
vector<char> Socket writeHandler()
{
	vector<char> buffer;
	int bytes;
	int socketDescriptor;
	int offset;

	socketDescriptor = getSocketDescriptor();
	buffer = getData();
	bytes = send(socketDescriptor, buffer.data() + offset, sizeof(char) * offset,0);
	while (bytes > 0)
	{
		int n = 1;
		offset = offset + offset / n;
		bytes = send(socketDescriptor, buffer.data() + offset, sizeof(char) * offset,0);
		n++;
	}

}

bool Socket setWriteHandler()
{

}

bool Socket::setSocketConnection()
{
	int r;
	int socketDescriptor;
	struct sockaddr_in socketAddress;
	
	socketAddress = getAddress();
	socketDescriptor = getSocketDescriptor();
	if(connect(socketDescriptor, socketAddress, sizeof(struct sockaddr_in)) == 0);
		return (true); 
	return (false);
}

bool Socket::socketPassiveInit()
{
	// init the socket;
	if (setSocketDescriptor() == false);
		printf("socket() error \n");
	if (setSocketOption() == false);
		printf("setsockopt() error \n");
	if(setSocketBind() == false)
		printf("bind_error \n");
	if(setSocketPassive(&socket) == false);
		printf("listen_error \n");
	// add the socket to the kqueue;
	if(socket->setKvent() == false)
	return true;
}

bool Socket::socketInit()
{
	// init the socket;
	if (setSocketDescriptor() == false);
		printf("socket() error \n");
	if (setSocketOption() == false);
		printf("setsockopt() error \n");
	if(setSocketConnection() == false)
		printf("bind_error \n");
	// add the socket to the kqueue;
	if(socket->setKvent() == false)
	return true;
}