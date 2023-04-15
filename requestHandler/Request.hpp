#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

typedef s_request Request;

enum methodType {
	GET = 0,
	POST,
	DELETE,
	NA
};

struct request_line {
	methodType	method;
	std::string	uri;
	std::string	httpVersion;
};

struct	general_header {
	std::string	cache;
	std::string	connection;
	std::string	date;
	std::string	pragma;
	std::string	trailer;
	std::string	transferEncoding;
};

struct	request_header {
	std::string	accept;
	std::string	acceptCharset;
	std::string	acceptEncoding;
	std::string	acceptLanguage;
	std::string authorization;
	std::string	expect;
	std::string	host;
	std::string	proxy_auth;
	std::string	range;
	std::string	referer;
	std::string	userAgent;
};

struct	entity_header {
	std::string	allow;
	std::string	contentEncoding;
	std::string	contentLanguage;
	std::string	contentLength;
	std::string	contentLocation;
	std::string	contentMd;
	std::string	contentRange;
	std::string	contentType;
	std::string	expires;
	std::string	lastModified;
};

struct s_request {
	request_line	rline;
	general_header	gheader;
	request_header	rheader;
	entity_header	eheader;
};



#endif