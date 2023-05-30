#include "Socket.hpp"
#include "stdlib.h"

Socket::Socket()
: requestIsComplete_(false)
{
}

Socket::Socket(int port, struct sockaddr_in servAddr)
: port_(port)
, sourceAddress_(servAddr)
{
	requestIsComplete_= false;
	requestLength_ = 0;
	setConnectionTimer();
}

Socket::Socket(int port, struct sockaddr_in servAddr, int fd)
: port_(port)
, serverSd_(fd)
, sourceAddress_(servAddr)
{
	requestIsComplete_= false;
	requestLength_ = 0;
}

Socket::~Socket()
{
}

Socket &Socket::operator= (const Socket& other)
{
	port_= other.port_;
	serverSd_ = other.serverSd_;
	clientSd_ = other.clientSd_;
	sourceAddress_ = other.sourceAddress_;
	destinationAddress_ = other.destinationAddress_;
	data_ = other.data_;
	event_ = other.event_;
	nEvent_= other.nEvent_;
	return (*this);
}

// SET---------------------------------------------------------

void Socket::setConnectionTimer()
{
	connectionTimer_ = time(NULL);
}

time_t Socket::getConnectionTimer()
{
	return (connectionTimer_);
}

bool Socket::setSocketDescriptor()
{
	int socketDescriptor;

	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor == -1)
		return false;
	clientSd_ = socketDescriptor;
	return (true);
}

bool Socket::setSocketOption()
{
	int val;
	int statusFnctl;

	val = 1;
	statusFnctl = fcntl(clientSd_, F_SETFL, O_NONBLOCK);
	if (statusFnctl == -1)
	{
  		perror("calling fcntl");
	}
	if( setsockopt(clientSd_, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int)) == -1)
		return (false);
	return (true);
}

bool Socket::setSocketBind()
{
	int retValue;
	socklen_t addrLen;

	addrLen = sizeof(sourceAddress_);
	retValue = bind(clientSd_, (sockaddr *)(&sourceAddress_), addrLen);
	return (retValue == 0 ? true : false);
}

bool Socket::setSocketPassive()
{
	int retValue;

	retValue = listen(clientSd_, 5);
	return(retValue == 0 ? true : false);
}

std::ostream& operator<<(std::ostream& out, struct kevent event)
{
	out << "ident: " << event.ident << "\n"
		<< "filter: " << event.filter << "\n"
		<< "flags: " << event.flags << "\n"
		<< "fflags: " << event.fflags << "\n"
		<< "data: " << event.data << "\n";
	return out;
}

bool Socket::setKeventForWrite()
{
	EV_SET(&(events_[1]), clientSd_, EVFILT_WRITE,  EV_ADD, 0, 0, 0);
	if (kevent(kqFd, events_, 1, NULL, 0, NULL) == -1)
		return false;
	return true;
}

bool Socket::setKevent()
{
	// EV_SET(&(events_[0]), clientSd_, EVFILT_READ,  EV_ADD | EV_CLEAR, 0, 0, 0);
	// EV_SET(&(events_[1]), clientSd_, EVFILT_WRITE,  EV_ADD | EV_CLEAR, 0, 0, 0);
	EV_SET(&(events_[0]), clientSd_, EVFILT_READ,  EV_ADD, 0, 0, 0);
	EV_SET(&(events_[1]), clientSd_, EVFILT_WRITE,  EV_ADD, 0, 0, 0);
	if (kevent(kqFd, events_, 2, NULL, 0, NULL) == -1)
		return false;
	return true;
}

void Socket::setResponse(std::string response)
{
	response_ = response;
}

std::string Socket::getResponse()
{
	return(response_);
}

bool Socket::unsetKevent(int filter)
{
	if( filter == EVFILT_READ)
		EV_SET(&(events_[0]), clientSd_, EVFILT_READ, EV_DELETE, 0, 0, 0);
	else 
		EV_SET(&(events_[1]), clientSd_, EVFILT_WRITE,  EV_DELETE, 0, 0, 0);
	if (kevent(kqFd, events_, 1, NULL, 0, NULL) == -1)
		return false;
	return true;
}

void Socket::setDestinationAddress (struct sockaddr_in address)
{
	destinationAddress_ = address;
}

bool Socket::setSocketConnection()
{
	socklen_t destAddrLen = sizeof(destinationAddress_);
	
	memset(&destinationAddress_, 0, sizeof(destinationAddress_));
	clientSd_ = accept(serverSd_, reinterpret_cast<sockaddr*>(&destinationAddress_), &destAddrLen);
	if (clientSd_ >= 0)
	{
		connectionUp_ = true;
		return (true);
	}
	connectionUp_ = false;
	return (false);
}

// GET----------------------------------------------------
int Socket::getPort()
{
	return (port_);
}

struct kevent Socket::getEvent()
{
	return (event_);
}

struct kevent* Socket::getEvents()
{
	return (events_);
}

int Socket::getSocketDescriptor()
{
	return (clientSd_);
}

std::string Socket::getData()
{
	return data_;
}

struct sockaddr_in& Socket::getSocketDestinationAddress()
{
	return (destinationAddress_);
}

struct sockaddr_in& Socket::getSocketSourceAddress()
{
	return (sourceAddress_);
}

bool Socket::getRequestStatus()
{
	return(requestIsComplete_);
}

void Socket::setRequestStatus(bool status)
{
	requestIsComplete_ = status;
}

bool Socket::getConnectionStatus()
{
	return(connectionUp_);
}

int Socket::closeConnection()
{
	close(clientSd_);
	requestIsComplete_ = false;
	connectionUp_ = false;
	return (0);
}

size_t Socket::getContentLength()
{
	size_t contentLenght;
	int index;
	std::string substring( "Content-Length:");
	index = data_.find(substring);
	if(index == -1)
		return (0);
	contentLenght = atoi((&data_[index + substring.size()]));
	return (contentLenght);
}

static size_t getHeaderLength(std::string string)
{
	int index = 0;

	std::string substring ("\r\n\r\n");
	index = string.find(substring);
	if(index == -1)
		return(string.size());
	return(index + 4);
}

void Socket::setRequestLength()
{
	size_t contentLength = getContentLength();
	headerLength_ = getHeaderLength(data_);//data_.size();
	// if the request don't have Content-Lenght the request is exactly data_.size();
	requestLength_ = contentLength + headerLength_;
}

int Socket::readHandler(size_t sizeToRead)
{
	char *buffer;
	int bytes;

	buffer = (char *)malloc(sizeToRead + 1);
	bytes = recv(clientSd_, buffer, sizeToRead, 0);
	if(bytes == 0) //close Connection
	{
			// std::cout << "connection being closed for: " << clientSd_ << "from CLIENT side" << std::endl;
		// return(closeConnection());
	}
	else if (bytes < 0)// && errno == EAGAIN)
	{
		std::cerr << "ERROR: SOCKET PROBABLY BLOCKING" << std::endl;
		return (-1);
	}
	buffer[bytes] = '\0';
	data_.append(buffer, bytes);
	if(requestLength_ == 0)
		setRequestLength();
	if(data_.size() >= requestLength_)
		requestIsComplete_ = true; //here we set the request as complete
	free(buffer);
	return (1);
}

// we should sett an offset;
bool Socket::writeHandler(std::string response)
{
	int bytes;
	if (connectionUp_ == false)
		return false;

	bytes = send(clientSd_, response.c_str(), response.size(), 0);
	if (bytes < 0)
		return false;
	response_ = response_.substr(bytes);
	std::cout << "RESPONSE SIZE AFTER WRITE: " << response_.size() << std::endl;
	if (response_.empty())
	{
		// we want to reset the request_value in the socket, in order to handle the next;
		data_ = "";
		requestLength_ = 0;
		requestIsComplete_ = false;
		setRequestStatus(false);
		closeConnection();
		std::cout << "connection being closed for: " << clientSd_ << "from SERVER side" << std::endl;
	}
	return true;
}

bool Socket::socketPassiveInit()
{
	if (setSocketDescriptor() == false)
		printf("socket() error \n");
	if (setSocketOption() == false)
		printf("setsockopt() error \n");
	if(setSocketBind() == false)
		printf("bind_error \n");
	if(setSocketPassive() == false)
		printf("listen_error \n");
	if(setKevent() == false)
		return false;
	return true;
}

bool Socket::socketInit()
{
	if(setSocketConnection() == false)
		printf("socketConnection() error \n");
	if(setKevent() == false)
		printf("kevent() list error \n");
	return true;
}