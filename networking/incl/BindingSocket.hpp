#ifndef BindingSocket_hpp
#define BindingSocket_hpp

#include "Socket.hpp"

namespace webserv {
	class BindingSocket: public Socket {
		private:
			int binding_;
		public:
			BindingSocket(int domain, int service, int protocol, int port, u_long interface);
			void	establishConnection(int socket, struct sockaddr_in saddr);
	};
};

#endif