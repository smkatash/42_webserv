# ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"
#include <fcntl.h>


class Socket 
{
	private:

	int port_;
	int sD_;
	int clientSd_;
	struct sockaddr_in sourceAddress_; //it stay the same of the Server.
	struct sockaddr_in destinationAddress_; //it's always different.

	std::vector<char> data_;

	struct kevent event_;
	// struct kevent events_[MAX_EVENTS];
	struct kevent events_[2];
	
	int nEvent_; //number of the events returned by kevent;

	public: //init bla bla 
		Socket(int domain);
		Socket(int domain, struct sockaddr_in servAdd);
		Socket(const Socket &other);
		Socket &Socket::operator= (Socket other);
		~Socket();

		// Socket &operator=(const Socket &other);

	public: //method
		bool setSocketDescriptor();
		bool setSocketOption();
		bool setSocketBind();
		bool setSocketPassive();
		bool setKevent();
		void setAddress ();
		bool setData (std::vector<char> buffer);

		int getPort();
		struct kevent getEvent();
		int getSocketDescriptor();
		std::vector<char> getData();
		struct sockaddr_in getSocketDestinationAddress();
		struct sockaddr_in getSocketSourceAddress();

		bool socketInit();
		bool socketPassiveInit();
		bool readHandler();
		bool writeHandler();
};




#endif