#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>

struct request_line {
	std::string	method;
	std::string	uri;
	std::string	httpVersion;

	friend std::ostream& operator<<(std::ostream& os, const request_line& rl) {
		os << "method: " << rl.method << std::endl;
    	os << "uri: " << rl.uri << std::endl;
    	os << "httpVersion: " << rl.httpVersion << std::endl;
    	return os;
	}
};

struct	general_header {
	std::string	cache;
	std::string	connection;
	std::string	date;
	std::string	pragma;
	std::string	trailer;
	std::string	transferEncoding;
	std::string	upgrade;
	std::string	via;
	std::string	warning;

	// output for debugging
	friend std::ostream& operator<<(std::ostream& os, const general_header& gh) {
		os << "cache: " << gh.cache << std::endl;
		os << "connection: " << gh.connection << std::endl;
		os << "date: " << gh.date << std::endl;
		os << "pragma: " << gh.pragma << std::endl;
		os << "trailer: " << gh.trailer << std::endl;
		os << "transferEncoding: " << gh.transferEncoding << std::endl;
		os << "upgrade: " << gh.upgrade << std::endl;
		os << "via: " << gh.via << std::endl;
		os << "warning: " << gh.warning << std::endl;
		return os;
	}
};

struct	request_header {
	std::string	accept;
	std::string	acceptCharset;
	std::string	acceptEncoding;
	std::string	acceptLanguage;
	std::string authorization;
	std::string	expect;
	std::string	from;
	std::string	host;
	std::string	ifMatch;
	std::string	ifModified;
	std::string	ifNone;
	std::string	ifRange;
	std::string	ifUnmodified;
	std::string	maxForwards;
	std::string	proxy_auth;
	std::string	range;
	std::string	cookie;
	std::string	referer;
	std::string	te;
	std::string	userAgent;

	// output for debugging
	friend std::ostream& operator<<(std::ostream& os, const request_header& rh) {
		os << "accept: " << rh.accept << std::endl;
		os << "acceptCharset: " << rh.acceptCharset << std::endl;
		os << "acceptEncoding: " << rh.acceptEncoding << std::endl;
		os << "acceptLanguage: " << rh.acceptLanguage << std::endl;
		os << "authorization: " << rh.authorization << std::endl;
		os << "expect: " << rh.expect << std::endl;
		os << "from: " << rh.from << std::endl;
		os << "host: " << rh.host << std::endl;
		os << "ifMatch: " << rh.ifMatch << std::endl;
		os << "ifModified: " << rh.ifModified << std::endl;
		os << "ifNone: " << rh.ifNone << std::endl;
		os << "ifRange: " << rh.ifRange << std::endl;
		os << "ifUnmodified: " << rh.ifUnmodified << std::endl;
		os << "maxForwards: " << rh.maxForwards << std::endl;
		os << "proxy_auth: " << rh.proxy_auth << std::endl;
		os << "range: " << rh.range << std::endl;
		os << "cookie: " << rh.cookie << std::endl;
		os << "referer: " << rh.referer << std::endl;
		os << "te: " << rh.te << std::endl;
		os << "userAgent: " << rh.userAgent << std::endl;
		return os;
	}
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

	// output for debugging
	friend std::ostream& operator<<(std::ostream& os, const entity_header& eh) {
		os << "allow: " << eh.allow << std::endl;
		os << "contentEncoding: " << eh.contentEncoding << std::endl;
		os << "contentLanguage: " << eh.contentLanguage << std::endl;
		os << "contentLength: " << eh.contentLength << std::endl;
		os << "contentLocation: " << eh.contentLocation << std::endl;
		os << "contentMd: " << eh.contentMd << std::endl;
		os << "contentRange: " << eh.contentRange << std::endl;
		os << "contentType: " << eh.contentType << std::endl;
		os << "expires: " << eh.expires << std::endl;
		os << "lastModified: " << eh.lastModified << std::endl;
		return os;
	}
};

struct s_request {
	request_line				rline;
	generalHeader				gheader;
	request_header				rheader;
	entity_header				eheader;
	std::vector<std::string>	rbody;
};

typedef	s_request		Request;

#endif