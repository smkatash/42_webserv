#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "../requestHandler/Request.hpp"

/* TODO most common status codes, may be add more later*/
#define	OK				200		// "200 OK" /* Request is okay, entity body contains requested resource. */
#define	CREATED			201		// "201 Created" /* For requests that create server objects (e.g., PUT). */

#define	MOVEDPERMAN		301		// "301 Moved Permanently" /* page moved to a different url*/
#define	FOUND			302		// "302 Found" /* used for temporary redirect */
#define	UNAUTHORIZED	400		// "400 Unauthorized" /* unauthenticated request */
#define NOTFOUND		404		// "404 Not Found" /* requested page not found */
#define GONE			410		// "410 Gone" /* like 404, but temporary*/
#define	INTERNALERROR	500		// "500 Internal Server Error" /* generic server error */
#define	BADGATEWAY		502		// "502 Bad Gateway" /* invalid response from another server */


struct response_line {
	std::string version;
	std::string statusCode;
	std::string reasonPhrase;
	
	// Debugging Purposes
	friend std::ostream& operator<<(std::ostream& os, const response_line& resLine) {
		os << "version......: " << resLine.version << std::endl;
		os << "status code..: " << resLine.statusCode << std::endl;
		os << "reason phrase: " << resLine.reasonPhrase << std::endl;
		return os;
	}
};

struct response_header {
	std::string age;
	std::string publicMethods;
	std::string retryAfter;
	std::string server;
	std::string title;
	std::string warning;
	std::string acceptRanges;
	std::string vary;
	std::string proxyAuth;
	std::string	cookie;
	std::string wwwAuth;

	// Debugging Purposes
	friend std::ostream& operator<<(std::ostream& os, const response_header& rh) {
		os << "age............: " << rh.age << std::endl;
		os << "publicMethods..: " << rh.publicMethods << std::endl;
		os << "retryAfter.....: " << rh.retryAfter << std::endl;
		os << "server.........: " << rh.server << std::endl;
		os << "title..........: " << rh.title << std::endl;
		os << "warning........: " << rh.warning << std::endl;
		os << "acceptRanges...: " << rh.acceptRanges << std::endl;
		os << "vary...........: " << rh.vary << std::endl;
		os << "proxyAuth......: " << rh.proxyAuth << std::endl;
		os << "cookie.........: " << rh.cookie << std::endl;
		os << "wwwAuth........: " << rh.wwwAuth << std::endl;
		return os;
	}
};

struct s_response {
	response_line				rline;
	general_header				gheader;
	response_header				rheader;
	entity_header				eheader;
	char*						rbody;
};

typedef s_response	Response;

#endif
