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
};

struct response_header {
	std::string acceptRanges;
	std::string age;
	std::string	eTag;
	std::string location;
	std::string retryAfter;
	std::string proxyAuth;
	std::string server;
	std::string vary;
	std::string wwwAuth;
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
