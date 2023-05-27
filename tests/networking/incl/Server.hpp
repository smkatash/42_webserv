#ifndef Server_hpp
#define Server_hpp

#include "hdSockets.hpp"

namespace webserv {
	class Server {
		protected:
			ListeningSocket	*sockfd_;
		private:
			virtual	void	accepter() = 0;
			virtual	void	handler() = 0;
			virtual	void	responder() = 0;
		public:
			Server(int domain, int service, int protocol, int port, u_long interface, int backlog);
			virtual	void launch() = 0;
			ListeningSocket	*getSocket();
	};
};

#endif