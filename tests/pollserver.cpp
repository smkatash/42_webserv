#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#define PORT "9034"
#define BACKLOG 10

int	get_listening_socket(void) {
	int sfd;
	int yes = 1; // For setsockopt() SO_REUSEADDR, below
	int rvalue;
	struct addrinfo hints, *result, *ptr;

	/* Get address, socket and bind it */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	rvalue = getaddrinfo(NULL, PORT, &hints, &result);
	if (rvalue != 0) {
		std::cerr << "server: " << gai_strerror(rvalue);
		exit(1);
	}

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sfd < 0) {
			continue;
		}
		setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) < 0) {
			close(sfd);
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ptr == nullptr)
		return -1;
	if (listen(sfd, BACKLOG) == -1)
		return -1;
	return sfd;
}

void	add_to_pfds(struct pollfd* pfds[], int new_fd, int *fd_count, int *fd_size) {
	 // If we don't have room, add more space in the pfds array
	if (*fd_count == *fd_size) { 
		*fd_size *= 2; // Double it
		*pfds = (struct pollfd*)realloc(*pfds, sizeof(**pfds) * (*fd_size)); 
	}
	(*pfds)[*fd_count].fd = new_fd;
	(*pfds)[*fd_count].events = POLL_IN;

	(*fd_count)++;
}

void *getaddrinfo_helper(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void	free_fd_pfds(struct pollfd pfds[],int i, int *fd_count) {
	/* copy from the end */
	pfds[i] = pfds[*fd_count -1];
	(*fd_count)--;
}

int main(void) {
	int socket_fd;
	int	new_fd;
	struct sockaddr_storage clients;
	socklen_t	socklen;
	char	message[256];
	char	remoteIP[INET6_ADDRSTRLEN];

	/* Start off with room for 5 connections,realloc as necessary */
	int	fd_size = 5;
	struct pollfd *pfds = (struct pollfd *)malloc(sizeof(struct pollfd) * fd_size);

	/* Set up and get a listening socket */
	socket_fd = get_listening_socket();
	if (socket_fd == -1) {
		std::cerr << "listen to the socket failed\n";
		exit(1);
	}

	/* add listener to the set*/
	int	fd_count = 1;
	pfds[0].fd = socket_fd;
	pfds[0].events = POLLIN; /* ready to read on incoming connection */

	for (;;) {
		int poll_count = poll(pfds, fd_count, -1);
		if (poll_count == -1) {
			perror("poll failed");
			exit(1);
		}
		/* Run through the existing connections looking for data to read */
		for (int i = 0; i < fd_count; i++) {
			/* check if someone is ready to read */
			if (pfds[i].revents && POLL_IN) {
				if (pfds[i].fd == socket_fd) {
					/* listener is ready to read, handle new connection */
					socklen = sizeof(clients);
					/* extracts the first connection request on the queue 
					of pending connections for the listening socket, sockfd, 
					creates a new connected socket */
					new_fd = accept(socket_fd, (struct sockaddr *)&clients, &socklen);
					if (new_fd == -1) {
						perror("accept failed");
					} else {
						add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);
						std::cout << "pollserver: connection with " << \
							inet_ntop(clients.ss_family, getaddrinfo_helper((struct sockaddr*)&clients), remoteIP, INET6_ADDRSTRLEN) << \
							" on socket " << new_fd << std::endl;
					}
				} else {
					/* not listener */
					int nbytes = recv(pfds[i].fd, message, sizeof(message), 0);
					int sender_fd = pfds[i].fd;

					if (nbytes <= 0) {
						if (nbytes == 0) {
							std::cout << "socket " << socket_fd << " hung up\n";
						} else {
							perror("failed to receive");
						}
						close(pfds[i].fd);
						free_fd_pfds(pfds, i, &fd_count);
					} else {
						/* no errors */
						for (int j = 0; j < fd_count; j++) {
							int dest_fd = pfds[j].fd;
							if (dest_fd != socket_fd && dest_fd != sender_fd) {
								if (send(dest_fd, message, nbytes, 0) == -1) {
									perror("send failed");
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}