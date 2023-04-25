#include <map>
#include "ResponseHandler.hpp"

#define RESPONSE_LINE	0
#define GENERAL_HEADER	1
#define RESPONSE_HEADER	2
#define ENTITY_HEADER	3
#define BODY			4

ResponseHandler::ResponseHandler() {
	res_.rbody = nullptr;
}

ResponseHandler::~ResponseHandler() {}

/*
	I know this is crappy code. This is why I suggest we use vectors or some
	container instead of using structs. But if we must, then this is the only way
	as I can't loop through struct members.
*/

void	ResponseHandler::fillResponseLine(response_line* rline) {
	res_.rline.reasonPhrase = rline->reasonPhrase;
	res_.rline.statusCode = rline->statusCode;
	res_.rline.version = rline->version;
}

void	ResponseHandler::fillGeneralHeader(general_header* gheader) {
	res_.gheader.cache = gheader->cache;
	res_.gheader.connection = gheader->connection;
	res_.gheader.date = gheader->date;
	res_.gheader.pragma = gheader->pragma;
	res_.gheader.trailer = gheader->trailer;
	res_.gheader.transferEncoding = gheader->transferEncoding;
	res_.gheader.upgrade = gheader->upgrade;
	res_.gheader.via = gheader->via;
	res_.gheader.warning = gheader->warning;
}

void	ResponseHandler::fillResponseHeader(response_header* rheader) {
	res_.rheader.age = rheader->age;
	res_.rheader.publicMethods = rheader->publicMethods;
	res_.rheader.retryAfter = rheader->retryAfter;
	res_.rheader.server = rheader->server;
	res_.rheader.title = rheader->title;
	res_.rheader.warning = rheader->warning;
	res_.rheader.acceptRanges = rheader->acceptRanges;
	res_.rheader.vary = rheader->vary;
	res_.rheader.proxyAuth = rheader->proxyAuth;
	res_.rheader.cookie = rheader->cookie;
	res_.rheader.wwwAuth = rheader->wwwAuth;
}

void	ResponseHandler::fillEntityHeader(entity_header* eheader) {
	res_.eheader.allow = eheader->allow;
	res_.eheader.contentEncoding = eheader->contentEncoding;
	res_.eheader.contentLanguage = eheader->contentLanguage;
	res_.eheader.contentLength = eheader->contentLength;
	res_.eheader.contentLocation = eheader->contentLocation;
	res_.eheader.contentMd = eheader->contentMd;
	res_.eheader.contentRange = eheader->contentRange;
	res_.eheader.contentType = eheader->contentType;
	res_.eheader.expires = eheader->expires;
	res_.eheader.lastModified = eheader->lastModified;
}

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

/*
	You have to fill one of these structs and you give it to the function along with 
	an identifier indicating which struct:

	response_line
	general_header
	response_header
	entity_header
*/
void ResponseHandler::fill(int identifier, void *header) {
	switch (identifier)
	{
	case RESPONSE_LINE:
		fillResponseLine((response_line*)header);
		break;
	case GENERAL_HEADER:
		fillGeneralHeader((general_header*)header);
		break;
	case RESPONSE_HEADER:
		fillResponseHeader((response_header*)header);
		break;
	case ENTITY_HEADER:
		fillEntityHeader((entity_header*)header);
		break;
	case BODY:
		res_.rbody = (char*)header;
		break;
	default:
		break;
	}
}

std::string ResponseHandler::getResponse() {
	std::string	header;
	header = appendResponseLine() + appendGeneralHeader()
				+ appendResponseHeader() + appendEntityHeader();
	if (res_.rbody != nullptr)
		header.append('\n' + res_.rbody);
	return header;
}
