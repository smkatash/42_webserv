#include "Socket.hpp"
#include "color.hpp"
#include <stdlib.h>
#include <arpa/inet.h>

#define SOCKET_VERBOSE

//////////////////////////////////////////////////// static helper:
static size_t getHeaderLength(std::string string);

static int errorOut(std::string message, int returnValue)
{
	printError(message, 0);
	return (returnValue);
}

int Socket::retry()
{
	static int counter;
	counter++;
	if(counter != 5)
		return (errorOut("ERROR: SOCKET PROBABLY BLOCKING... RETRY", -1));
	else
		return (errorOut("ERROR: SOCKET PROBABLY BLOCKING... RETRY", 0));
}
//////////////////////////////////////////////////// canonic methods:
Socket::Socket() :
requestIsComplete_(false)
{
}

Socket::Socket(int port, struct sockaddr_in servAddr) :
port_(port),
sourceAddress_(servAddr)
{
	requestIsComplete_= false;
	requestLength_ = 0;
	setConnectionTimer();
}

Socket::Socket(int port, struct sockaddr_in servAddr, int fd, ConfigFile serverConfig): port_(port),
serverSd_(fd),
sourceAddress_(servAddr),
serverConfiguration_(serverConfig)
{
	requestIsComplete_= false;
	requestLength_ = 0;
	setConnectionTimer();

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
	return (*this);
}

//////////////////////////////////////////////////// set methods:
void Socket::setConnectionTimer() { connectionTimer_ = time(NULL); }

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
	EV_SET(&(events_[0]), clientSd_, EVFILT_READ,  EV_ADD, 0, 0, 0);
	EV_SET(&(events_[1]), clientSd_, EVFILT_WRITE,  EV_ADD, 0, 0, 0);
	if (kevent(kqFd, events_, 2, NULL, 0, NULL) == -1)
		return false;
	return true;
}

void Socket::setResponse(std::string response) { response_ = response; }

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

bool Socket::setSocketConnection()
{
	socklen_t destAddrLen = sizeof(destinationAddress_);
	
	memset(&destinationAddress_, 0, sizeof(destinationAddress_));
	clientSd_ = accept(serverSd_, reinterpret_cast<sockaddr*>(&destinationAddress_), &destAddrLen);
	if (clientSd_ >= 0)
	{
		setConnectionStatus(true);
		return (true);
	}
	setConnectionStatus(false);
	return (false);
}

void Socket::setRequestLength()
{
	size_t contentLength = getContentLength();
	headerLength_ = getHeaderLength(data_);
	requestLength_ = contentLength + headerLength_;
}

void Socket::setRequestStatus( bool status) { requestIsComplete_ = status; }
void Socket::setDestinationAddress( struct sockaddr_in address) { destinationAddress_ = address; }
void Socket::setConnectionStatus( bool status) { connectionUp_ = status; }

//////////////////////////////////////////////////// get methods:
int Socket::getPort() { return (port_); }
int Socket::getSocketDescriptor() { return (clientSd_); }
bool Socket::getConnectionStatus() { return (connectionUp_); }
bool Socket::getRequestStatus() { return (requestIsComplete_); }
time_t Socket::getConnectionTimer() { return (connectionTimer_); }
ConfigFile Socket::getServerConfiguration() { return (serverConfiguration_); }
std::string Socket::getResponse() { return(response_); }
std::string Socket::getData() {return (data_); }
struct kevent Socket::getEvent() { return (event_); }
struct kevent* Socket::getEvents() { return (events_); }
struct sockaddr_in& Socket::getSocketDestinationAddress() { return (destinationAddress_); }
struct sockaddr_in& Socket::getSocketSourceAddress() { return (sourceAddress_); }

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

//////////////////////////////////////////////////// member functions:
static size_t getHeaderLength(std::string string)
{
	int index = 0;

	std::string substring ("\r\n\r\n");
	index = string.find(substring);
	if(index == -1)
		return(string.size());
	return(index + 4);
}

int Socket::closeConnection()
{
	close(clientSd_);
	connectionUp_ = false;
	return (0);
}

int Socket::connectionClosedClientSide()
{
	#ifdef SOCKET_VERBOSE
		printAction("ACTION:\033[38;5;189m CLIENT\033[38;5;197m Connection closed \t\t\033[38;5;49m| CLIENT addr:\t", (int) ntohl(destinationAddress_.sin_addr.s_addr));
	#endif
	data_ = "";
	requestLength_ = 0;
	requestIsComplete_ = false;
	setConnectionStatus(false);
	setRequestStatus(false);
	return(closeConnection());
}

int Socket::closingConnectionServerSide()
{
	data_ = "";
	requestLength_ = 0;
	requestIsComplete_ = false;
	setRequestStatus(false);
	setConnectionStatus(false);
	#ifdef SOCKET_VERBOSE
		printAction("ACTION: \033[38;5;45mSERVER\033[38;5;197m Connection closed \t\t\033[38;5;49m| SOCKET fdes:\t" ,clientSd_);
	#endif
	return (closeConnection());
}

void Socket::reset()
{
	data_ = "";
	requestLength_ = 0;
	requestIsComplete_ = false;
	setRequestStatus(false);
	#ifdef SOCKET_VERBOSE
		printAction("ACTION: \033[38;5;45mSERVER\033[38;5;229m reset()\tvalues \t\t\t\033[38;5;49m| SOCKET fdes:\t" ,clientSd_);
	#endif
	return ;
}


int Socket::readHandler(size_t sizeToRead)
{
	setConnectionTimer();
	char *buffer;
	int bytes;

	buffer = new char[sizeToRead + 1];
	bytes = recv(clientSd_, buffer, sizeToRead, 0);
	printAction("ACTION: \033[38;5;45mSERVER\033[38;5;229m recv()\trequest\t\t\t\033[38;5;49m| CLIENT addr:\t",(int) ntohl(destinationAddress_.sin_addr.s_addr));
	if(bytes == 0)
		return (connectionClosedClientSide());
	else if (bytes < 0)
	{
		if(retry() == -1) //we retry 5 tymes;
			return (-1);
		else
			return (closingConnectionServerSide());
	}
	buffer[bytes] = '\0';
	data_.append(buffer, bytes);
	if(requestLength_ == 0)
		setRequestLength();
	if(data_.size() >= requestLength_)
	{
		setRequestStatus(true);
		requestLength_ = 0;
	}
	delete[] buffer;
	return (1);
}


bool Socket::writeHandler(std::string response, bool closeConnection)
{
	setConnectionTimer();

	int bytes;
	if (connectionUp_ == false)
		return false;

	bytes = send(clientSd_, response.c_str(), response.size(), 0);
	printAction("ACTION: \033[38;5;45mSERVER\033[38;5;229m send()\tresponse \t\t\033[38;5;49m| CLIENT addr:\t", ntohl(destinationAddress_.sin_addr.s_addr));
	if (bytes < 0)
		return false;
	response_ = response_.substr(bytes);
	if (response_.empty() && closeConnection == false)
		reset();
	if (response_.empty() && closeConnection == true)
		closingConnectionServerSide();
	return true;
}

#ifdef SOCKET_VERBOSE
bool Socket::socketPassiveInit()
{
	if (setSocketDescriptor() == false)
		return( printError("ERROR: socket() \t\t\t| SERVER port: ", port_));
	if (setSocketOption() == false)
		return( printError("ERROR: setsockopt() \t\t\t| SERVER port: ", port_));
	if (setSocketBind() == false)
		return( printError("ERROR: bind() \t\t\t| SERVER port: ", port_));
	if (setSocketPassive() == false)
		return(  printError("ERROR: connect() \t\t\t| SERVER port: ", port_));
	if (setKevent() == false)
		return( printError("ERROR: kevent() \t\t\t| SERVER port: ", port_));
	printAction("ACTION: server listening ...\t\t\t| SERVER port: ", port_);
	return true;
}

bool Socket::socketInit()
{
	if(setSocketConnection() == false)
		return(printError( "ERROR: socketConnection() \t| SOCKET port: ", port_));
	if(setKevent() == false)
		return(printError("ERROR: kevent() \t\t\t| SOCKET port: ", port_));
	printAction("ACTION: \033[38;5;45mSERVER\033[38;5;229m Accepted new connection \t\t\033[38;5;49m| SOCKET fdes:\t", clientSd_);
	return true;
}

#else
bool Socket::socketPassiveInit()
{
	if (setSocketDescriptor() == false)
		return(false);
	if (setSocketOption() == false)
		return(false);
	if (setSocketBind() == false)
		return(false);
	if (setSocketPassive() == false)
		return(false);
	if (setKevent() == false)
		return(false);
	printAction("ACTION: server listening ...\t\t\t| SERVER port: ", port_);
	return true;
}

bool Socket::socketInit()
{
	if(setSocketConnection() == false)
		return(false);
	if(setKevent() == false)
		return(false);
	printAction("ACTION: \033[38;5;45mSERVER\033[38;5;229m Accepted new connection \t\t\033[38;5;49m| SOCKET fdes:\t", clientSd_);
	return true;
}

#endif