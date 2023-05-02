#include <map>
#include <algorithm>
#include "ResponseHandler.hpp"
#include "parser_utils.hpp" // Methods enum
#include <fstream>
#include <string>

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
	{
		header += '\n';
		header.append(res_.rbody);
	}
	return header;
}



bool	ResponseHandler::isMethodAllowed(Methods method, std::vector<int> methodsAllowed)
{
	if (methodsAllowed.empty())
		return true;
	std::vector<int>::iterator it = std::find(methodsAllowed.begin(), methodsAllowed.end(), method);
	if (it == methodsAllowed.end())
	{
		setCode("405");
		return false;
	}
	return true;
}

void	ResponseHandler::setResponseBody(std::string fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open() || !file.good())
		return setCode("404");
	std::string temp;
	std::string body;
	while (std::getline(file, temp))
		body += temp + '\n';
	res_.rbody = (char*)body.c_str();
	return setCode("200");
}

void	ResponseHandler::setCode(std::string code)
{
	res_.rline.statusCode = code;
	if (code == "200")
		res_.rline.reasonPhrase = "OK";
	if (code == "404")
		res_.rline.reasonPhrase = "Not Found";
	if (code == "405")
		res_.rline.reasonPhrase = "Not Allowed";
}

void ResponseHandler::directoryRequest(const t_endpoint& loc)
{
	// if (loc.lautoindex)
	// 	;// Do autoindex
	// else if (loc.lindex.empty())
	// 	return setCode("404");
	// if (loc.lindex.empty())
	// 	return setCode("404");
	std::string uri = loc.lindex.front(); // TODO: Needs changing to work with index vector
	if (loc.lroot.empty())
		uri.insert(0, conf_.getRoot(""));
	else
		uri.insert(0, loc.lroot);
	std::cout << uri << std::endl;
	return setResponseBody(uri);
}

bool Pred(char a, char b)
{
	if (a == b && a == '/')
		return 1;
	return 0;
}

std::string removeDuplicateSlashes(const std::string& str)
{
	std::string res = str;
	std::string::iterator last = res.begin();
	last = std::unique(res.begin(), res.end(), &Pred);
	res.erase(last, res.end());
	return res;
}

std::string ResponseHandler::getUriEndpoint(const std::string& uri)
{
	if (uri == "")
		return "";
	std::string ep = conf_.getEndPoint(uri);
	if (ep == "")
		return getUriEndpoint(uri.substr(0, uri.find_last_of('/'))); // Here we make a substring from the start to '/' (basically with every function call we step back a directory)
	return ep;
}

/* Prepares URI to be opened as a file by appending it to the root folder,
	removing a forward slash from the end, and adding a '.' to the front */
void ResponseHandler::prepUriFile(std::string& uri, const t_endpoint& loc)
{
	if (loc.lroot.empty())
		uri.insert(0, conf_.getRoot(""));
	else
		uri.insert(0, loc.lroot);
	if (uri.back() == '/')
		uri.erase(uri.end());
	if (uri.front() == '/')
		uri.insert(0, ".");
}

void ResponseHandler::get()
{
	std::string uri = removeDuplicateSlashes(req_.rline.uri);
	std::string ep = getUriEndpoint(uri);
	if (ep == "")
		return setCode("404");
	t_endpoint loc = conf_.getLocation(ep);
	if (!isMethodAllowed(GET, loc.lmethod))
		setCode("405");
	if (uri == ep)	// If the URI matches with the endpoint then we know it's a directory
		return directoryRequest(loc);
	prepUriFile(uri, loc);
	return setResponseBody(uri);
}
