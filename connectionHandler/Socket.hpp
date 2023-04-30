# ifndef SOCKET_HPP
# define SOCKET_HPP


#define MAX_EVENTS 10

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <stdint.h>
#include <machine/types.h>
#include <arpa/inet.h>



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


// listen(sockfd, SOMAXCONN)

// fd_set my_set;
// fd_set wk_set;

// FD_ZERO(&my_set);         /* initialize  fd_set */
// FD_SET(sock_fd, &my_set)  /* put listener into fd_set */
// max_fd = sockfd;

// while( TRUE )
// {
//     memcpy(&wk_set, &my_set, sizeof(my_set));

//     rc = select(max_fd + 1, &wk_set, NULL, NULL, NULL )
//     if ( rc == -1 )
//     {
//         if ( errno == EINTR )
//             continue;
//         printf("select failed: errno = %d\n", errno);
//         exit(1);
//     }
//     for(i = 0; i < max_fd; i++) /* check everything in wk_set */
//     {
//         if ( FD_ISSET(i, &wk_set)
//         {
//             if ( i == sockfd )  /* is it the listener socket?? */
//             {
//                 wk_sock=accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
//                 if ( wk_sock == -1 )
//                 {
//                     printf("accept failed: errno=%d\n", errno);
//                     exit(1);
//                 }
//                 FD_SET(wk_sock, &my_set);  /* put into fd_set */
//                 if ( wk_sock > max_sock )
//                     max_sock = wk_sock;
//             }else{
//                 /* ready to read from this socket */
//                 recv_len = recv(i, .... )
//                 if ( recv_len == 0 ) /* client disconnected */
//                 {
//                     close(i);
//                     FD_CLR(i, &my_set);
//                     if (i == max_fd )
//                     {
//                         for(x=0;x<max_fd;x++)  /* find new max_fd */
//                         {
//                             if ( FD_ISSET(x, &my_set )
//                                 max_fd = x;
//                         }
//                     }
//                 } else { 
//                 handle message from client
//                 }
//             }
//         }
//     }
// }