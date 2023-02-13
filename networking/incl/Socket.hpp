#ifndef Socket_hpp
#define Socket_hpp

#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>

namespace webserv {
	
	class Socket {
		private:
			int	sfd_;
			struct sockaddr_in saddr_;

		public:
			Socket(int domain, int service, int protocol, int port, u_long interface);
			void 		testConnection(int);
			virtual void establishConnection(int socket, struct sockaddr_in saddr) = 0;

			int					getSocket(void);
			struct sockaddr_in	getSaddress(void);
	};
};

#endif