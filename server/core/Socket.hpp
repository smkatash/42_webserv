# ifndef SOCKET_HPP
# define SOCKET_HPP

#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <unistd.h>

#include "ConfigFile.hpp"

extern int kqFd;

class Socket 
{
	private:

	int port_;
	int serverSd_;
	int clientSd_;
	time_t connectionTimer_;
	struct sockaddr_in sourceAddress_;
	struct sockaddr_in destinationAddress_;

	std::string data_;
	std::string response_;

	size_t requestLength_;
	size_t headerLength_;

	bool connectionUp_;
	bool requestIsComplete_;
	bool chunkedRequest_;

	ConfigFile serverConfiguration_;
	struct kevent event_;
	struct kevent events_[2];

	public: 
		Socket();
		Socket(int port, struct sockaddr_in servAddr);
		Socket(int port, struct sockaddr_in servAddr, int fd, ConfigFile serverConfig);
		Socket &operator= (const Socket& other);
		~Socket();

		bool                setSocketDescriptor();
		bool                setSocketOption();
		bool                setSocketBind();
		bool                setSocketPassive();
		bool                setSocketConnection();
		bool                setKevent();
		bool                setKeventListeningSocket();
		bool                unsetKevent(int filter);
		void                setResponse(std::string response);
		void                setDestinationAddress(struct sockaddr_in address);
		void                setAddress();
		void                setRequestLength();
		void                setConnectionTimer();
		void                setChunkedOpt(bool val);

		int                 getPort();
		int                 getSocketDescriptor();
		bool                getConnectionStatus();
		bool                getChunkedOpt();
		bool                getRequestStatus();
		time_t              getConnectionTimer();
		size_t              getContentLength();
		ConfigFile          getServerConfiguration();
		std::string         getResponse();
		std::string         getData();
		std::string&        getDataRef();
		struct kevent       getEvent();
		struct kevent*      getEvents();
		struct sockaddr_in& getSocketDestinationAddress();
		struct sockaddr_in& getSocketSourceAddress();

		int closeConnection();
		int retry();
		int closingConnectionServerSide();
		int connectionClosedClientSide();
		int readHandler(size_t sizeToRead);
		bool writeHandler(std::string response, bool closeConnection);
		bool socketPassiveInit();
		bool socketInit();
		void reset();

};

std::ostream& operator<<(std::ostream& out, struct kevent event);

#endif