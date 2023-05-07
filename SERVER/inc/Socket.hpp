# ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserver.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

class Socket 
{
	private:

	int port_;
	int sD_;
	int clientSd_;
	struct sockaddr_in serverAddress_; //it stay the same of the Server.
	struct sockaddr_in clientAddress_; //it's always different.

	vector<char> data_;

	struct kevent event_;
	// struct kevent events_[MAX_EVENTS];
	struct kevent events_[2];
	
	int nEvent_; //number of the events returned by kevent;

	public: //init bla bla 
		Socket(int domain);
		Socket(int domain, int serverSd);
		Socket(const Socket &other);
		~Socket();

		Socket &operator=(const Socket &other);

	public: //method
		bool setSocketDescriptor();
		bool setSocketOption()
		bool setSocketBind();
		bool setSocketPassive()
		bool setKevent()
		void setAddress ()

		std::string setReadHandler();
		boool setWriteHandler();

		int getPort();
		void getEvent();
		int getServerSd();
		struct sockaddr_in Socket::getSocketAddress();
	
		bool socketInit();


};




#endif