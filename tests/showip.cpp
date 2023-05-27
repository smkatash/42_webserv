#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <iostream>

// struct addrinfo {
// 	int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
// 	int	ai_family;	/* PF_xxx */
// 	int	ai_socktype;	/* SOCK_xxx */
// 	int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
// 	socklen_t ai_addrlen;	/* length of ai_addr */
// 	char	*ai_canonname;	/* canonical name for hostname */
// 	struct	sockaddr *ai_addr;	/* binary address */
// 	struct	addrinfo *ai_next;	/* next structure in linked list */
// };


/*
 * [XSI] Structure used by kernel to store most addresses.
 */
// struct sockaddr {
// 	__uint8_t       sa_len;         /* total length */
// 	sa_family_t     sa_family;      /* [XSI] address family */
// 	char            sa_data[14];    /* [XSI] addr value (actually larger) */
// };

/*
 * Socket address, internet style.
 */
// struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };

/*
 * Internet address (a structure for historical reasons)
 */
// struct in_addr {
// 	in_addr_t s_addr;
// };

// #ifndef _IN_ADDR_T
// #define _IN_ADDR_T
// #include <machine/types.h> /* __uint32_t */
// typedef __uint32_t      in_addr_t;      /* base type for internet address */
// #endif /* _IN_ADDR_T */

int main(int argc, char *argv[]) {
	struct addrinfo hints, *res, *p; 
	int status;

	char ipstr[INET6_ADDRSTRLEN];
	if (argc != 2) {
		std::cerr << "usage: showip hostname\n"; 
		return 1;
	}
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version hints.ai_socktype = SOCK_STREAM;
	
	// Get the info
	if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) { 
		std::cerr << "getaddrinfo: %s\n", gai_strerror(status);
		return 2;
	}

	std::cout << "IP addresses " << argv[1] << std::endl;
	for(p = res; p != NULL; p = p->ai_next) { 
		void *addr;
		std::string ipver;
		// get the pointer to the address itself, // different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr; 
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr; 
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}
	// convert the IP to a string and print it:
	inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
	std::cout << " " << ipver << " " << ipstr << std::endl;
	std::cout << p->ai_addrlen << " " << p->ai_protocol << " ";
	std::cout << p->ai_socktype << " " << p->ai_flags << " " << p->ai_family << std::endl;
	std::cout << p->ai_addr->sa_data << " " << p->ai_addr->sa_family << " " << p->ai_addr->sa_len << std::endl;
	std::cout << "----------" << std::endl;
	}
	freeaddrinfo(res); // free the linked list return 0;
	return 0;
}