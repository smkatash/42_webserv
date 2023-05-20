# ifndef SOCKET_HPP
# define SOCKET_HPP

#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <unistd.h>

extern int kqFd;

class Socket 
{
	private:

	int port_;
	int serverSd_;
	int clientSd_;
	struct sockaddr_in sourceAddress_; //it stay the same of the Server.
	struct sockaddr_in destinationAddress_; //it's always different.

	std::string response_;
	std::string data_;

	struct kevent event_;
	// struct kevent events_[MAX_EVENTS];
	struct kevent events_[2];
	
	int nEvent_; //number of the events returned by kevent;

	public: //init bla bla 
		Socket();
		Socket(int port, struct sockaddr_in servAdd);
		Socket(int port, struct sockaddr_in servAdd, int fd);
		// Socket(const Socket &other);
		Socket &operator= (const Socket& other);
		~Socket();

		// Socket &operator=(const Socket &other);

	public: //method
		bool setSocketDescriptor();
		void setResponse(std::string response);
		bool setSocketOption();
		bool setSocketBind();
		bool setSocketPassive();
		bool setSocketConnection();
		bool setKevent();
		bool unsetKevent(int filter);
		void setDestinationAddress (struct sockaddr_in address);
		void setAddress ();
		bool setData (std::vector<char> buffer);

	
		int 				getPort();
		std::string 		getResponse();
		struct kevent 		getEvent();
		struct kevent*		getEvents();
		int					getSocketDescriptor();
		std::string 		getData();
		struct sockaddr_in&	getSocketDestinationAddress();
		struct sockaddr_in&	getSocketSourceAddress();

		bool socketInit();
		bool socketPassiveInit();
		int readHandler();
		bool writeHandler(std::string response);
};

std::ostream& operator<<(std::ostream& out, struct kevent event);



#endif