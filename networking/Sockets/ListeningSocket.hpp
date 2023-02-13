#ifndef ListeningSocket_hpp
#define ListeningSocket_hpp

#include "BindingSocket.hpp"

namespace webserv {
	class ListeningSocket: public BindingSocket {
		private:
			int	backlog_;
			int	listenfd_;
		public:
			ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog);
			void	startListening();
	};
};

#endif