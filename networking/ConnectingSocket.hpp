#ifndef ConnectingSocket_hpp
#define ConnectingSocket_hpp

#include "Socket.hpp"

namespace webserv {
	class ConnectingSocket: public Socket {
		public:
			ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);
			int	establishConnection(int socket, struct sockaddr_in saddr);
	};
};

#endif