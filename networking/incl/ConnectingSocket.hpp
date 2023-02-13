#ifndef ConnectingSocket_hpp
#define ConnectingSocket_hpp

#include "Socket.hpp"

namespace webserv {
	class ConnectingSocket: public Socket {
		private:
			int connect_;
		public:
			ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);
			void	establishConnection(int socket, struct sockaddr_in saddr);
	};
};

#endif