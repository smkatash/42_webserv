// struct kevent {
//     §§            uintptr_t ident;        /* identifier for this event */
//             uint32_t  filter;       /* filter for event */
//             uint32_t  flags;        /* action flags for kqueue */
//             uint32_t  fflags;       /* filter flag value */
//             int64_t   data;         /* filter data value */
//             void     *udata;        /* opaque user data identifier */
// };
# ifndef SOCKET_HPP
# define SOCKET_HPP

#include "Socket.hpp"
#include <sys/socket.h>

class Socket
{
    private:

    int port;
    int addr;
    int addrIn;
    int sockFd;
    int sockAddrLen;

    public:
        Socket(int domain, int type, int protocol);
        Socket(const Socket &other);
        ~Socket();

        Socket &operator=(const Socket &other);
        // bool operator==(int );

        int getSocketFd() const;
        int getAddress() const;
        int getPort() const;
};

#endif
