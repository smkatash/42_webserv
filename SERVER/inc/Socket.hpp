# ifndef SOCKET_HPP
# define SOCKET_HPP

class Socket 
{
	private:
	// sd = socketDescriptor
	int port_;
	int serverSd_;
	int clientSd_;
	struct sockaddr_in serverAddress_;
	struct sockaddr_in clientAddress_;

	struct kevent event_;
	struct kevent events_[MAX_EVENTS];
	
	int nEvent_; //number of the events returned by kevent;

	public:
		Socket(int domain, int kqFd);
		Socket(int domain, int kqFd), int serverSd);
		Socket(const Socket &other);
		~Socket();

		Socket &operator=(const Socket &other);

		bool setServerSd();
		void setServerAddress();
		bool setSocketFd(int fd);
		int getPort();
		void getKqueue();
		void getEvent();
		int getServerSd();
		struct sockaddr_in Socket::getAddress(int n);
};

bool socketInit(Socket &Socket);
bool socketBind(Socket &Socket);


#endif