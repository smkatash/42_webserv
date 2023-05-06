#include <map>
#include <algorithm>
#include <fstream>
#include <string>
#include "ResponseHandler.hpp"
#include "parser_utils.hpp" // Methods enum
#include "response_utils.hpp"

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: req_(req), conf_(conf)
{
	res_.rbody = nullptr;
	res_.rline.version = "HTTP/1.1";
	res_.rheader.server = "Francesco's Pizzeria/2.0 (MacOS)";
}

ResponseHandler::~ResponseHandler() {}

std::string ResponseHandler::responseLine()
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

std::string ResponseHandler::generalHeader()
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

std::string ResponseHandler::responseHeader()
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

std::string ResponseHandler::entityHeader()
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

std::string ResponseHandler::getResponse()
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

void	ResponseHandler::setCode(int code)
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

bool	ResponseHandler::isMethodAllowed(Methods method, std::vector<int> methodsAllowed)
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

std::string ResponseHandler::findUriEndpoint(const std::string& uri)
{
	if (uri == "")
		return "/";
	std::string ep = conf_.getEndPoint(uri);
	if (ep == "")
		return findUriEndpoint(uri.substr(0, uri.find_last_of('/'))); // Here we make a substring from the start until '/' (basically with every function call we step back a directory)
	return ep;
}

/* Prepares URI to be opened as a file by appending it to the root folder,
	removing a forward slash from the end, and adding a '.' to the front */
void ResponseHandler::prepUriFile(std::string& uri, const t_endpoint& loc)
{
	if (uri.front() == '/')
		uri.erase(uri.begin());
	if (loc.lroot.empty())
		uri.insert(0, conf_.getRoot(""));
	else
		uri.insert(0, loc.lroot);
	if (uri.back() == '/')
		uri.erase(uri.end());
	if (uri.front() == '/')
		uri.insert(0, ".");
}

void	ResponseHandler::setResponseBody(std::string fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open() || !file.good())
		return setCode(404);
	std::string temp;
	std::string body;
	while (std::getline(file, temp))
		body += temp + '\n';
	res_.rbody = (char*)body.c_str();
	res_.eheader.contentLength = toString(body.length());
	file.close();
	return setCode(200);
}

void ResponseHandler::getReturnResponse(t_endpoint loc)
{
	if (isNumber(loc.lredirect.substr(0, 3)))
	{
		res_.rbody = (char*)loc.lredirect.substr(4).c_str(); // TODO: Test this. What would happen.
		res_.eheader.contentLength = (char*)loc.lredirect.substr(4).length();
		return setCode(strtoi(loc.lredirect.substr(0, 3)));
	}
	else if (loc.lredirect.substr(0, 4) == "http")
	{
		res_.rheader.location = loc.lredirect;
		return setCode(302);
	}
	return setCode(500);
}

void ResponseHandler::getAutoIndexResponse(t_endpoint loc, std::string ep)
{
	std::string templateFile;
	if (loc.lroot.empty())
		templateFile = initAutoIndex(ep, conf_.getRoot(""));
	else
		templateFile = initAutoIndex(ep, loc.lroot);
	if (templateFile.empty())
		return setCode(404);
	res_.rbody = (char*)templateFile.c_str();
	res_.eheader.contentType = findContentType(".html");
	res_.eheader.contentLength = toString(templateFile.size());
	return setCode(200);
}

void ResponseHandler::getDirResponse(t_endpoint loc, std::string ep)
{
	if (loc.lautoindex)
		return getAutoIndexResponse(loc, ep);
	else if (loc.lindex.empty())
		return setCode(404);
	std::string uri = findUsableFile(loc.lindex, ep + "/");
	if (uri.empty())
		return setCode(404);
	uri = ep + '/' + uri;
	return getNormalResponse(loc, uri);
}

void ResponseHandler::getNormalResponse(t_endpoint loc, std::string uri)
{
	prepUriFile(uri, loc);
	std::string uriPath = uri.substr(0, uri.find('?'));
	res_.eheader.contentType = findContentType(uriPath.substr(uri.find_last_of('.')));
	return setResponseBody(uriPath);
}

void ResponseHandler::get()
{
	try
	{
		res_.gheader.date = findCurrentTimeGMT();
		std::string uri = removeDuplicateSlashes(req_.rline.uri);
		std::string ep = findUriEndpoint(uri);
		t_endpoint loc = conf_.getLocation(ep); // try-catch because getLocation may throw an exception

		if (!isMethodAllowed(GET, loc.lmethod))
			return setCode(405);
		
		/* TODO: Check if you have to send to cgi handler by checking if
		there's a query and seeing if there's a cgi specified in location */

		if (!loc.lredirect.empty())
			return getReturnResponse(loc);
		if (uri == ep)	// If the URI matches with the endpoint then we know it's a directory
			return getDirResponse(loc, ep);
		return getNormalResponse(loc, uri);
	}
	catch(const std::exception& e)
	{
		return setCode(404);
	}
}
