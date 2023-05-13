#include "../inc/Core.hpp"

Core::Core ()
{
	Server server();
	if(server.serverInit())
		serverClose();
	this->server_ = server;

	// void eventWrite(Server server);
	// void eventRead(Server server);
	// this->servers_ = servers;
}

/*
		int     kevent( int	kq, const struct kevent	*changelist, int nchanges, 
 						struct	kevent *eventlist, int nevents,
						const struct timespec *timeout);
*/

bool Core::setNewConnection()
{
		// is a new connection;
	Socket newSocket();
	if(newSocket.init(server_.getPort(), server_.getServerAddress()) == false)
	{
		printf("error socket init");
		return (false);
	}
	if(newSocket.setSocketConnection() == false)
	{
		printf("error setting connection");
		return (false);
	}
	if (server_.appendNewToSocketList(newSocket) == false);
	{
		printf("error append new socket");
		return (false);
	}

	return (true);
}

Core::run()
{
	int i;
	int tmpFileDescriptor;

	while(1)
	{
		i = 0;
		int numOfEvent = kevent(kqFd, NULL, 0, eventlist_, MAX_EVENTS, NULL);
		while( i <= numOfevent)
		{
			currentEvent = eventlist_[i];
			tmpEventDescriptor = currentEvent.ident;
			if (tmpEventDescriptor == server_.getServerFd()) //here we should check a vector of serverFd;
			{
				if(setNewConnection() == false)
				{
					printf("error setting new connection");
					exit();
				}
			}
			else //receive the message;
			{
				ssize_t n = recv(tmpEventDescriptor, buffer, BUFFER_SIZE, 0);
				if (n == -1)
				{
					printf("Failed recv() call");
					close(tmpEventDescriptor);
					continue;
				} 
				else if (n == 0) 
				{
					printf("Connection has been closed by remote client\n");
					close(tmpEventDescriptor);
					continue;
      			}
			}
			// write a response:
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!\r\n";
        ssize_t sent = send(tmpEventDescriptor, response, response.size, 0);
        if (sent == -1) {
          printf("Failed sent() call");
          close(tmpEventDescriptor);
          continue;
        }
			i++;
		}
	}
}
