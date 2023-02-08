#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

// /*
//  * Socket address, internet style.
//  */
// struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };

int main() {
	struct sockaddr_in sa; // IPv4
	struct sockaddr_in6 sa6; // IPv6
	std::cout << "from string to binary" << std::endl;
	inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4
	inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); // IPv6
	std::cout << sa.sin_addr.s_addr << std::endl;
	std::cout << sa6.sin6_addr.__u6_addr.__u6_addr32 << std::endl << std::endl;
	
	std::cout << "from binary to string" << std::endl;
	/**
	 * @brief INET_ADDRSTRLEN needs to be 16.
	 * The largest possible string representation of an IPv4 address consumes 15 bytes, 
	 * e.g. "255.255.255.255". Therefore 16 bytes are required to store such an IP 
	 * address with its terminating null character.
	 * @brief longtest IPv6 address takes 39 bytes, with IPv4 tunneling, the longest form can be 45 bytes:
	 * ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255
	 */
	char ip4[INET_ADDRSTRLEN];
	char ip6[INET6_ADDRSTRLEN]; // space to hold the IPv6 string
	inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
	inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);
	std::cout << ip4 << std::endl;
	std::cout << ip6 << std::endl;

}