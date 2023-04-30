
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_EVENTS 10

int main(int argc, char *argv[])
{
    int server_sd;
    int client_fd;
    int kq;
    int nev;
    int i, n;


    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct kevent event;
    struct kevent events[MAX_EVENTS];

    // Create a TCP socket &bind;
	//SOCKET SYSTEM CALL sd
    if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an IP address and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sd, SOMAXCONN) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Create a new kqueue
    if ((kq = kqueue()) < 0) {
        perror("kqueue failed");
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the kqueue
    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) < 0) 
    {
        perror("kevent (server) failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started\n");

    // Main loop
    while (1) 
    {
        // Wait for events
        nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (nev < 0) {
            perror("kevent failed");
            exit(EXIT_FAILURE);
        }

        // Handle events
        for (i = 0; i < nev; i++) 
        {
            // Check for errors
            if (events[i].flags & EV_ERROR) 
            {
                printf("Error on socket %d: %s\n", (int)events[i].ident, strerror(events[i].data));
                close((int)events[i].ident);
                continue;
            }

            // Accept new connections
            if (events[i].ident == server_sd) 
            {
                // Accept the new connection
                socklen_t client_len = sizeof(client_addr);
                if ((client_fd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len)) < 0) 
                {
                    perror("accept failed");
                    continue;
                }

                printf("Accepted new connection from %s:%d (socket %d)\n",
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);

                // Add the new client socket to the kqueue
                EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) < 0) 
                {
                    perror("kevent (client) failed");
                    close(client_fd);
                }
            }
            // Read data from clients
        }
    }
}


//---------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server_fd, client_fd, kq, nev, i;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    struct kevent events[MAX_EVENTS], event;
    char buffer[BUFFER_SIZE];

    // Create a TCP socket and bind it to a port
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    // Create a new kqueue
    kq = kqueue();
    if (kq == -1) 
    {
        perror("kqueue");
        exit(1);
    }

    // Add the server socket to the event queue
    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) 
    {
        perror("kevent");
        exit(1);
    }

    // Wait for events to occur
    while (1) 
    {
        nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (nev == -1) 
        {
            perror("kevent");
            exit(1);
        }

        // Handle all the events that occurred
        for (i = 0; i < nev; i++) 
        {
            // If the event is for a new connection, accept it and add the new client socket to the event queue
            if (events[i].ident == server_fd) 
            {
                client_len = sizeof(client_addr);
                client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) 
                {
                    perror("kevent");
                    exit(1);
                }
                printf("New client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
            // If the event is for an existing client socket, read data from it and echo it back
            else 
            {
                bzero(buffer, BUFFER_SIZE);
                if (read(events[i].ident, buffer, BUFFER_SIZE) == -1) 
                {
                    perror("read");
                    exit(1);
                }
                printf("Received message: %s\n", buffer);
                if (write(events[i].ident, buffer, strlen(buffer)) == -1) 
                {
                    perror("write");
                    exit(1);
                }
            }
        }
    }

    return 0;
}



// -------------------------------------------------------

// struct kevent {
//          uintptr_t       ident;          /* identifier for this event */
//          int16_t         filter;         /* filter for event */
//          uint16_t        flags;          /* general flags */
//          uint32_t        fflags;         /* filter-specific flags */
//          intptr_t        data;           /* filter-specific data */
//          void            *udata;         /* opaque user data identifier */
//  };

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

int
main(int argc, char** argv)
{
    /* A single kqueue */
    int kq = kqueue();
    /* Two kevent structs */
    struct kevent *ke = malloc(sizeof(struct kevent) * 2);

    /* Initialise one struct for the file descriptor, and one for SIGINT */
    int fd = open(argv[1], O_RDONLY);
    EV_SET(ke, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_DELETE | NOTE_RENAME, 0, NULL);
    signal(SIGINT, SIG_IGN);
    EV_SET(ke + 1, SIGINT, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);

    /* Register for the events */
    if(kevent(kq, ke, 2, NULL, 0, NULL) < 0)
        perror("kevent");

    while(1) {
        memset(ke, 0x00, sizeof(struct kevent));
        if(kevent(kq, NULL, 0, ke, 1, NULL) < 0)
            perror("kevent");

        switch(ke->filter)
        {
            /* File descriptor event: let's examine what happened to the file */
            case EVFILT_VNODE:
                printf("Events %d on file descriptor %d\n", ke->fflags, (int) ke->ident);

                if(ke->fflags & NOTE_DELETE)
                    printf("The unlink() system call was called on the file referenced by the descriptor.\n");
                if(ke->fflags & NOTE_WRITE)
                    printf("A write occurred on the file referenced by the descriptor.\n");
                if(ke->fflags & NOTE_EXTEND)
                    printf("The file referenced by the descriptor was extended.\n");
                if(ke->fflags & NOTE_ATTRIB)
                    printf("The file referenced by the descriptor had its attributes changed.\n");
                if(ke->fflags & NOTE_LINK)
                    printf("The link count on the file changed.\n");
                if(ke->fflags & NOTE_RENAME)
                    printf("The file referenced by the descriptor was renamed.\n");
                if(ke->fflags & NOTE_REVOKE)
                    printf("Access to the file was revoked via revoke(2) or the underlying fileystem was unmounted.");
                break;

            /* Signal event */
            case EVFILT_SIGNAL:
                printf("Received %s\n", strsignal(ke->ident));
                exit(42);
                break;

            /* This should never happen */
            default:
                printf("Unknown filter\n");
        }
    }
}