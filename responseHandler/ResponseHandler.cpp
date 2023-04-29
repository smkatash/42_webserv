#include <map>
#include "ResponseHandler.hpp"

#define RESPONSE_LINE	0
#define GENERAL_HEADER	1
#define RESPONSE_HEADER	2
#define ENTITY_HEADER	3
#define BODY			4

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: req_(req), conf_(conf) {
	res_.rbody = nullptr;
}

ResponseHandler::~ResponseHandler() {}

std::string ResponseHandler::appendResponseLine() {
	std::string line;
	if (!res_.rline.version.empty())
		line.append(res_.rline.version + '\n');
	if (!res_.rline.statusCode.empty())
		line.append(res_.rline.statusCode + '\n');
	if (!res_.rline.reasonPhrase.empty())
		line.append(res_.rline.reasonPhrase + '\n');
	return line;
}

std::string ResponseHandler::appendGeneralHeader() {
	std::string line;
	if (!res_.gheader.cache.empty())
		line.append(res_.gheader.cache + '\n');
	if (!res_.gheader.connection.empty())
		line.append(res_.gheader.connection + '\n');
	if (!res_.gheader.date.empty())
		line.append(res_.gheader.date + '\n');
	if (!res_.gheader.pragma.empty())
		line.append(res_.gheader.pragma + '\n');
	if (!res_.gheader.trailer.empty())
		line.append(res_.gheader.trailer + '\n');
	if (!res_.gheader.transferEncoding.empty())
		line.append(res_.gheader.transferEncoding + '\n');
	if (!res_.gheader.upgrade.empty())
		line.append(res_.gheader.upgrade + '\n');
	if (!res_.gheader.via.empty())
		line.append(res_.gheader.via + '\n');
	if (!res_.gheader.warning.empty())
		line.append(res_.gheader.warning + '\n');
	return line;
}

std::string ResponseHandler::appendResponseHeader() {
	std::string line;
	if (!res_.rheader.age.empty())
		line.append(res_.rheader.age + '\n');
	if (!res_.rheader.publicMethods.empty())
		line.append(res_.rheader.publicMethods + '\n');
	if (!res_.rheader.retryAfter.empty())
		line.append(res_.rheader.retryAfter + '\n');
	if (!res_.rheader.server.empty())
		line.append(res_.rheader.server + '\n');
	if (!res_.rheader.title.empty())
		line.append(res_.rheader.title + '\n');
	if (!res_.rheader.warning.empty())
		line.append(res_.rheader.warning + '\n');
	if (!res_.rheader.acceptRanges.empty())
		line.append(res_.rheader.acceptRanges + '\n');
	if (!res_.rheader.vary.empty())
		line.append(res_.rheader.vary + '\n');
	if (!res_.rheader.proxyAuth.empty())
		line.append(res_.rheader.proxyAuth + '\n');
	if (!res_.rheader.cookie.empty())
		line.append(res_.rheader.cookie + '\n');
	if (!res_.rheader.wwwAuth.empty())
		line.append(res_.rheader.wwwAuth + '\n');
	return line;
}

std::string ResponseHandler::appendEntityHeader() {
	std::string line;
	if (!res_.eheader.allow.empty())
		line.append(res_.eheader.allow + '\n');
	if (!res_.eheader.contentEncoding.empty())
		line.append(res_.eheader.contentEncoding + '\n');
	if (!res_.eheader.contentLanguage.empty())
		line.append(res_.eheader.contentLanguage + '\n');
	if (!res_.eheader.contentLength.empty())
		line.append(res_.eheader.contentLength + '\n');
	if (!res_.eheader.contentLocation.empty())
		line.append(res_.eheader.contentLocation + '\n');
	if (!res_.eheader.contentMd.empty())
		line.append(res_.eheader.contentMd + '\n');
	if (!res_.eheader.contentRange.empty())
		line.append(res_.eheader.contentRange + '\n');
	if (!res_.eheader.contentType.empty())
		line.append(res_.eheader.contentType + '\n');
	if (!res_.eheader.expires.empty())
		line.append(res_.eheader.expires + '\n');
	if (!res_.eheader.lastModified.empty())
		line.append(res_.eheader.lastModified + '\n');
	return line;
}

std::string ResponseHandler::getResponse() {
	std::string	header;
	header = appendResponseLine() + appendGeneralHeader()
				+ appendResponseHeader() + appendEntityHeader();
	if (res_.rbody != nullptr)
		header.append('\n' + res_.rbody);
	return header;
}

void ResponseHandler::getMethod() {
	if (req_.rline.uri == "/")
		
}
