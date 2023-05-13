
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



// --------------------------------------------------------

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <iostream>

using namespace std;

#define MAX_EVENTS 10

int main()
{
    int server_fd, client_fd, kq, nevents;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    struct kevent change_list[MAX_EVENTS], event_list[MAX_EVENTS];

    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Error creating socket." << endl;
        return 1;
    }

    // set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket to server address." << endl;
        close(server_fd);
        return 1;
    }

    // set socket to listen
    if (listen(server_fd, 10) < 0) {
        cerr << "Error setting socket to listen." << endl;
        close(server_fd);
        return 1;
    }

    // create kqueue
    kq = kqueue();
    if (kq < 0) {
        cerr << "Error creating kqueue." << endl;
        close(server_fd);
        return 1;
    }

    // add server_fd to kqueue
    EV_SET(&change_list[0], server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    while (true) {
        // wait for events
        nevents = kevent(kq, change_list, 1, event_list, MAX_EVENTS, NULL);

        // process events
        for (int i = 0; i < nevents; i++) {
            // handle new connection
            if (event_list[i].ident == server_fd) {
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
                if (client_fd < 0) {
                    cerr << "Error accepting new connection." << endl;
                    continue;
                }

                // set client_fd to non-blocking
                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                // add client_fd to kqueue
                EV_SET(&change_list[i], client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
            }
            // handle incoming data
            else {
                char buffer[1024];
                int nbytes = read(event_list[i].ident, buffer, sizeof(buffer));
                if (nbytes <= 0) {
                    // remove client_fd from kqueue
                    EV_SET(&change_list[i], event_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    close(event_list[i].ident);
                } else {
                    // process data
                    // ...
                }
            }
        }
    }

    return 0;
}



// ---------------------------------------------------------------------------------
/* Kernel Queue The Complete Guide: kqueue-connect.c: HTTP/1 client
Usage:
	$ nc -l 127.0.0.1 64000
	$ ./kqueue-connect
*/
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>

int kq;
int quit;

// the structure associated with a socket descriptor
struct context {
	int sk;
	void (*rhandler)(struct context *obj);
	void (*whandler)(struct context *obj);
	int data_offset;
};

void obj_write(struct context *obj);
void obj_read(struct context *obj);

void obj_prepare(struct context *obj)
{
	// create and prepare socket
	obj->sk = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	assert(obj->sk != -1);

	int val = 1;
	assert(0 == setsockopt(obj->sk, 0, TCP_NODELAY, (char*)&val, sizeof(int)));

	// attach socket to KQ
	struct kevent events[2];
	EV_SET(&events[0], obj->sk, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, obj);
	EV_SET(&events[1], obj->sk, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, obj);
	assert(0 == kevent(kq, events, 2, NULL, 0, NULL));
}

void obj_connect(struct context *obj)
{
	if (obj->whandler == NULL) {
		// begin asynchronous connection
		struct sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = ntohs(64000);
		char ip4[] = {127,0,0,1};
		*(int*)&addr.sin_addr = *(int*)ip4;

		int r = connect(obj->sk, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
		if (r == 0) {
			// connection completed successfully
		} else if (errno == EINPROGRESS) {
			// connection is in progress
			obj->whandler = obj_connect;
			return;
		} else {
			assert(0); // fatal error
		}

	} else {
		assert(errno == 0); // connection is successful
		obj->whandler = NULL; // we don't want any more signals from KQ
	}

	printf("Connected\n");
	obj_write(obj);
}

void obj_write(struct context *obj)
{
	const char data[] = "GET / HTTP/1.1\r\nHost: hostname\r\nConnection: close\r\n\r\n";
	int r = send(obj->sk, data + obj->data_offset, sizeof(data)-1 - obj->data_offset, 0);
	if (r > 0) {
		// sent some data
		obj->data_offset += r;
		if (obj->data_offset != sizeof(data)-1) {
			// we need to send the complete request
			obj_write(obj);
			return;
		}
		obj->whandler = NULL;

	} else if (r < 0 && errno == EAGAIN) {
		// the socket's write buffer is full
		obj->whandler = obj_write;
		return;
	} else {
		assert(0); // fatal error
	}

	printf("Sent HTTP request.  Receiving HTTP response...\n");
	obj_read(obj);
}

void obj_read(struct context *obj)
{
	char data[64*1024];
	int r = recv(obj->sk, data, sizeof(data), 0);
	if (r > 0) {
		// received some data
		printf("%.*s", r, data);
		obj_read(obj);
		return;

	} else if (r == 0) {
		// server has finished sending data

	} else if (r < 0 && errno == EAGAIN) {
		// the socket's read buffer is empty
		obj->rhandler = obj_read;
		return;
	} else {
		assert(0); // fatal error
	}

	quit = 1;
}

void main()
{
	// create KQ object
	kq = kqueue();
	assert(kq != -1);

	struct context obj = {};
	obj_prepare(&obj);
	obj_connect(&obj);

	// wait for incoming events from KQ and process them
	while (!quit) {
		struct kevent events[1];
		struct timespec *timeout = NULL; // wait indefinitely
		int n = kevent(kq, NULL, 0, events, 1, timeout);
		if (n < 0 && errno == EINTR)
			continue; // kevent() interrupts when UNIX signal is received
		assert(n > 0);

		// now process each signalled event
		for (int i = 0;  i != n;  i++) {
			struct context *o = events[i].udata;

			errno = 0;
			if (events[i].flags & EV_EOF)
				errno = events[i].fflags;

			if (events[i].filter == EVFILT_READ
				&& o->rhandler != NULL)
				o->rhandler(o); // handle read event

			if (events[i].filter == EVFILT_WRITE
				&& o->whandler != NULL)
				o->whandler(o); // handle write event
		}
	}

	close(obj.sk);
	close(kq);
}

// 

#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SERVER_PORT 8080
#define BACKLOG 10
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

int main() {
  int server_fd, kq, nev, conn_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];
  struct kevent event_list[MAX_EVENTS], current_event;

  // Create server socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Failed to create server socket");
    exit(EXIT_FAILURE);
  }

  // Bind server socket to port
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVER_PORT);
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    perror("Failed to bind server socket");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_fd, BACKLOG) == -1) {
    perror("Failed to listen for incoming connections");
    exit(EXIT_FAILURE);
  }

  // Create kqueue
  kq = kqueue();
  if (kq == -1) {
    perror("Failed to create kqueue");
    exit(EXIT_FAILURE);
  }

  // Register server socket for read events
  struct kevent event;
  EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
    perror("Failed to register server socket for read events");
    exit(EXIT_FAILURE);
  }

  // Event loop
  while (1) {
    // Wait for events
    nev = kevent(kq, NULL, 0, event_list, MAX_EVENTS, NULL);
    if (nev == -1) {
      perror("Failed to wait for events");
      exit(EXIT_FAILURE);
    }

    // Handle events
    for (int i = 0; i < nev; i++) {
      current_event = event_list[i];
      int fd = current_event.ident;
      if (fd == server_fd) {
        // Handle new connection
        conn_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (conn_fd == -1) {
          perror("Failed to accept incoming connection");
          continue;
        }

        // Register new connection socket for read events
        EV_SET(&event, conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
          perror("Failed to register new connection socket for read events");
          exit(EXIT_FAILURE);
        }
      } else {
        // Handle incoming data
        ssize_t n = recv(fd, buffer, BUFFER_SIZE, 0);
        if (n == -1) {
          perror("Failed to receive data");
          close(fd);
          continue;
        } else if (n == 0) {
          // Connection closed by remote client
          printf("Connection closed by remote client\n");
          close(fd);
          continue;
        }

        // Handle HTTP request
        // TODO: Implement HTTP request handling

        // Send response
        const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!\r\n";
        ssize_t sent = send(fd, response, strlen(response), 0);
        if (sent == -1) {
          perror("Failed to send response");
          close(fd);
          continue;
        }

        // Deregister connection socket from kqueue
        EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
          perror("Failed to deregister connection socket from kqueue");
          exit(EXIT_FAILURE);
        }

        // Close connection
        close(fd);
      }
    }
  }

  // Close server socket
  close(server_fd);

  return 0;
}