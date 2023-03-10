#ifndef TestServer_hpp
#define TestServer_hpp

#include "hdNetwork.hpp"
#include <istream>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#define MAXSIZE 3000
#define PORT 4950

namespace webserv {
	class TestServer : public Server {
		private:
			char	buff_[MAXSIZE] = {0};
			int		newsfd_;
			int		numbytes_;
			void	accepter();
			void	handler();
			void	responder();
		public:
			TestServer();
			void	launch();
			void	testTransfer(int bytes, ssize_t type);
	};

};

#endif