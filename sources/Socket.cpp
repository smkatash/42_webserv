#include "../include/Socket.hpp"
#include "../include/utils.hpp"

/*
int     socket(int domain, int type, int protocol);
domain = AF_INET for ip connection,
type = SOCK_STREA for the semantic,
    A stream socket must be in a connected state before any data may be sent or received on it.  A
     connection to another socket is created with a connect(2) or connectx(2) call.  Once connected, data may be transferred using read(2) and write(2) calls or some variant of
     the send(2) and recv(2) calls.  When a session has been completed a close(2) may be performed.  Out-of-band data may also be transmitted as described in send(2) and
     received as described in recv(2).
protocol = 0?

*/
bool Socket::init()
{
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if sockFd 
}

//-------------------CONTRUCTORS

Socket::Socket(int addr, int port) : int (addr), int (port)
{
    std::cout << "Socket CONSTRUCTOR" << std::endl;
};

Socket::~Socket()
{
    std::cout << "Socket DESTRUCTOR" << std::endl;
};

Socket::Socket(const Socket &other): int (other.addr), int (other.port), 
                                     int (other.addrIn), int (other.sockFd)
                                     int (other.sockAddrLen);
{
    std::cout << "Socket COPY_CONSTRUCTOR" << std::endl;
};

Socket &Socket::operator=(const Socket &other)
{
    if (this == other)
        return (*this);
    this.port = other.port;
    this.addr = other.addr;
    this.addrIn = other.addrIn;
    this.sockFd = other.sockFd;
    this.sockAddrLen = other.sockAddrLen;
    return (*this);
}

//-------------------GETTER
int  Socket::getPort() const
{ 
    return (port);
}

int  Socket::getAddress() const
{ 
    return (addr);
}

int  Socket::getAddressIn() const
{ 
    return (addrIn);
}

int  Socket::getSockFd() const
{ 
    return (sockFd);
}

int  Socket::getAddrLen() const
{ 
    return (sockAddrLen);
}

// struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };

bool Socket::init()
{
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
        return(printError("ERROR: socket creation failure", 1));
    m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = htonl(m_ip);
	m_address.sin_port = htons(m_port);
}

int Socket::accept_connect() 
{
	fd_type	fd = accept(
				m_sockfd,
				reinterpret_cast<sockaddr*>(&m_address),
				&m_add_len);
	return (fd);
}
