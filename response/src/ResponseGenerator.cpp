#include <map>
#include <algorithm>
#include <string>
#include "ResponseGenerator.hpp" // Methods enum
#include "parser_utils.hpp" // Methods enum
#include "response_utils.hpp"

ResponseGenerator::ResponseGenerator(Request& req, ConfigFile& conf)
: req_(req), conf_(conf)
{
	res_.rbody = nullptr;
	res_.rline.version = "HTTP/1.1";
	res_.rheader.server = "Francesco's Pizzeria/2.0 (MacOS)";
}

ResponseGenerator::~ResponseGenerator() {}

std::string ResponseGenerator::responseLine()
{
	std::string line;
	if (!res_.rline.version.empty())
		line.append(res_.rline.version + ' ');
	if (!res_.rline.statusCode.empty())
		line.append(res_.rline.statusCode + ' ');
	if (!res_.rline.reasonPhrase.empty())
		line.append(res_.rline.reasonPhrase + "\r\n");
	return line;
}

std::string ResponseGenerator::generalHeader()
{
	std::string line;
	if (!res_.gheader.cache.empty())
		line.append("Cache-Control: " + res_.gheader.cache + "\r\n");
	if (!res_.gheader.connection.empty())
		line.append("Connection: " + res_.gheader.connection + "\r\n");
	if (!res_.gheader.date.empty())
		line.append("Date: " + res_.gheader.date + "\r\n");
	if (!res_.gheader.pragma.empty())
		line.append("Pragma: " + res_.gheader.pragma + "\r\n");
	if (!res_.gheader.trailer.empty())
		line.append("Trailer: " + res_.gheader.trailer + "\r\n");
	if (!res_.gheader.transferEncoding.empty())
		line.append("Transfer-Encoding: " + res_.gheader.transferEncoding + "\r\n");
	if (!res_.gheader.upgrade.empty())
		line.append("Upgrade: " + res_.gheader.upgrade + "\r\n");
	if (!res_.gheader.via.empty())
		line.append("Via: " + res_.gheader.via + "\r\n");
	if (!res_.gheader.warning.empty())
		line.append("Warning: " + res_.gheader.warning + "\r\n");
	return line;
}

std::string ResponseGenerator::responseHeader()
{
	std::string line;
	if (!res_.rheader.acceptRanges.empty())
		line.append("Accept-Ranges: " + res_.rheader.acceptRanges + "\r\n");
	if (!res_.rheader.age.empty())
		line.append("Age: " + res_.rheader.age + "\r\n");
	if (!res_.rheader.eTag.empty())
		line.append("ETag: " + res_.rheader.eTag + "\r\n");
	if (!res_.rheader.location.empty())
		line.append("Location: " + res_.rheader.location + "\r\n");
	if (!res_.rheader.proxyAuth.empty())
		line.append("Proxy-Authenticate: " + res_.rheader.proxyAuth + "\r\n");
	if (!res_.rheader.retryAfter.empty())
		line.append("Retry-After: " + res_.rheader.retryAfter + "\r\n");
	if (!res_.rheader.server.empty())
		line.append("Server: " + res_.rheader.server + "\r\n");
	if (!res_.rheader.vary.empty())
		line.append("Vary: " + res_.rheader.vary + "\r\n");
	if (!res_.rheader.wwwAuth.empty())
		line.append("WWW-Authenticate: " + res_.rheader.wwwAuth + "\r\n");
	return line;
}

std::string ResponseGenerator::entityHeader()
{
	std::string line;
	if (!res_.eheader.allow.empty())
		line.append("Allow: " + res_.eheader.allow + "\r\n");
	if (!res_.eheader.contentEncoding.empty())
		line.append("Content-Encoding: " + res_.eheader.contentEncoding + "\r\n");
	if (!res_.eheader.contentLanguage.empty())
		line.append("Content-Language: " + res_.eheader.contentLanguage + "\r\n");
	if (!res_.eheader.contentLength.empty())
		line.append("Content-Length: " + res_.eheader.contentLength + "\r\n");
	if (!res_.eheader.contentLocation.empty())
		line.append("Content-Location: " + res_.eheader.contentLocation + "\r\n");
	if (!res_.eheader.contentMd.empty())
		line.append("Content-MD5: " + res_.eheader.contentMd + "\r\n");
	if (!res_.eheader.contentRange.empty())
		line.append("Content-Range: " + res_.eheader.contentRange + "\r\n");
	if (!res_.eheader.contentType.empty())
		line.append("Content-Type: " + res_.eheader.contentType + "\r\n");
	if (!res_.eheader.expires.empty())
		line.append("Expires: " + res_.eheader.expires + "\r\n");
	if (!res_.eheader.lastModified.empty())
		line.append("Last-Modified: " + res_.eheader.lastModified + "\r\n");
	return line;
}

bool	ResponseGenerator::isMethodAllowed(Methods method, std::vector<int> methodsAllowed)
{
	if (methodsAllowed.empty())
		return true;
	std::vector<int>::iterator it = std::find(methodsAllowed.begin(), methodsAllowed.end(), method);
	if (it == methodsAllowed.end())
	{
		setCode(405);
		return false;
	}
	return true;
}

void	ResponseGenerator::setCode(int code)
{
	res_.rline.statusCode = toString(code);
	if (code == 200)
		res_.rline.reasonPhrase = "OK";
	if (code == 302)
		res_.rline.reasonPhrase = "Found";
	if (code == 404)
		res_.rline.reasonPhrase = "Not Found";
	if (code == 405)
		res_.rline.reasonPhrase = "Not Allowed";
	if (code == 500)
		res_.rline.reasonPhrase = "Internal Server Error";
}

std::string ResponseGenerator::generateResponse()
{
	std::string	header;
	header = responseLine() + generalHeader()
				+ responseHeader() + entityHeader();
	if (res_.rbody != nullptr)
	{
		header.append("\r\n");
		header.append(std::string(res_.rbody));
	}
	return header;
}
