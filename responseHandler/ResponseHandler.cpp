#include <map>
#include <algorithm>
#include "ResponseHandler.hpp"
#include "parser_utils.hpp" // Methods enum
#include <fstream>
#include <string>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string getCurrentTimeGMT()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

	std::tm* gmTimeNow = std::gmtime(&timeNow);

	std::stringstream ss("");
	ss << std::put_time(gmTimeNow, "%a, %d %b %Y %H:%M:%S GMT");
	return ss.str();
}

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: req_(req), conf_(conf)
{
	res_.rbody = nullptr;
	res_.rline.version = "HTTP/1.1";
	res_.rheader.server = "Francesco's Pizzaria/2.0 (MacOS)";
}

ResponseHandler::~ResponseHandler() {}

std::string ResponseHandler::responseLine() {
	std::string line;
	if (!res_.rline.version.empty())
		line.append(res_.rline.version + ' ');
	if (!res_.rline.statusCode.empty())
		line.append(res_.rline.statusCode + ' ');
	if (!res_.rline.reasonPhrase.empty())
		line.append(res_.rline.reasonPhrase + '\n');
	return line;
}

std::string ResponseHandler::generalHeader() {
	std::string line;
	if (!res_.gheader.cache.empty())
		line.append("Cache-Control: " + res_.gheader.cache + '\n');
	if (!res_.gheader.connection.empty())
		line.append("Connection: " + res_.gheader.connection + '\n');
	if (!res_.gheader.date.empty())
		line.append("Date: " + res_.gheader.date + '\n');
	if (!res_.gheader.pragma.empty())
		line.append("Pragma: " + res_.gheader.pragma + '\n');
	if (!res_.gheader.trailer.empty())
		line.append("Trailer: " + res_.gheader.trailer + '\n');
	if (!res_.gheader.transferEncoding.empty())
		line.append("Transfer-Encoding: " + res_.gheader.transferEncoding + '\n');
	if (!res_.gheader.upgrade.empty())
		line.append("Upgrade: " + res_.gheader.upgrade + '\n');
	if (!res_.gheader.via.empty())
		line.append("Via: " + res_.gheader.via + '\n');
	if (!res_.gheader.warning.empty())
		line.append("Warning: " + res_.gheader.warning + '\n');
	return line;
}

std::string ResponseHandler::responseHeader() {
	std::string line;
	if (!res_.rheader.acceptRanges.empty())
		line.append("Accept-Ranges: " + res_.rheader.acceptRanges + '\n');
	if (!res_.rheader.age.empty())
		line.append("Age: " + res_.rheader.age + '\n');
	if (!res_.rheader.eTag.empty())
		line.append("ETag: " + res_.rheader.eTag + '\n');
	if (!res_.rheader.location.empty())
		line.append("Location: " + res_.rheader.location + '\n');
	if (!res_.rheader.proxyAuth.empty())
		line.append("Proxy-Authenticate: " + res_.rheader.proxyAuth + '\n');
	if (!res_.rheader.retryAfter.empty())
		line.append("Retry-After: " + res_.rheader.retryAfter + '\n');
	if (!res_.rheader.server.empty())
		line.append("Server: " + res_.rheader.server + '\n');
	if (!res_.rheader.vary.empty())
		line.append("Vary: " + res_.rheader.vary + '\n');
	if (!res_.rheader.wwwAuth.empty())
		line.append("WWW-Authenticate: " + res_.rheader.wwwAuth + '\n');
	return line;
}

std::string ResponseHandler::entityHeader() {
	std::string line;
	if (!res_.eheader.allow.empty())
		line.append("Allow: " + res_.eheader.allow + '\n');
	if (!res_.eheader.contentEncoding.empty())
		line.append("Content-Encoding: " + res_.eheader.contentEncoding + '\n');
	if (!res_.eheader.contentLanguage.empty())
		line.append("Content-Language: " + res_.eheader.contentLanguage + '\n');
	if (!res_.eheader.contentLength.empty())
		line.append("Content-Length: " + res_.eheader.contentLength + '\n');
	if (!res_.eheader.contentLocation.empty())
		line.append("Content-Location: " + res_.eheader.contentLocation + '\n');
	if (!res_.eheader.contentMd.empty())
		line.append("Content-MD5: " + res_.eheader.contentMd + '\n');
	if (!res_.eheader.contentRange.empty())
		line.append("Content-Range: " + res_.eheader.contentRange + '\n');
	if (!res_.eheader.contentType.empty())
		line.append("Content-Type: " + res_.eheader.contentType + '\n');
	if (!res_.eheader.expires.empty())
		line.append("Expires: " + res_.eheader.expires + '\n');
	if (!res_.eheader.lastModified.empty())
		line.append("Last-Modified: " + res_.eheader.lastModified + '\n');
	return line;
}

std::string ResponseHandler::getResponse() {
	std::string	header;
	header = responseLine() + generalHeader()
				+ responseHeader() + entityHeader();
	if (res_.rbody != nullptr)
	{
		header.append("\n");
		header.append(std::string(res_.rbody));
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

template <typename T>
std::string to_string(const T& value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

// std::string findContentType(std::ifstream file)
// {

// }

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
	res_.eheader.contentLength = to_string(body.length());
	// res_.eheader.contentType = findContentType(file);
	file.close();
	return setCode("200");
}

/* When URI is a directory */
void ResponseHandler::uriDirResponse(const t_endpoint& loc, std::string ep)
{
	if (loc.lautoindex)
	{
		std::string templateFile;
		if (loc.lroot.empty())
			templateFile = initAutoIndex(ep, conf_.getRoot(""));
		else
			templateFile = initAutoIndex(ep, loc.lroot);
		std::cout << loc.lroot << std::endl;
		if (templateFile.empty())
			return setCode("404");
		res_.rbody = (char*)templateFile.c_str();
		res_.eheader.contentLength = to_string(templateFile.size());
		return setCode("200");
	}
	else if (loc.lindex.empty())
		return setCode("404");
	std::string uri = loc.lindex.front(); // TODO: Needs changing to work with index vector
	uri = ep + '/' + uri;
	prepUriFile(uri, loc);
	return setResponseBody(uri);
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
	if (res.back() == '/' && res.length() != 1)
		res.pop_back();
	return res;
}

std::string ResponseHandler::getUriEndpoint(const std::string& uri)
{
	if (uri == "")
		return "/";
	std::string ep = conf_.getEndPoint(uri);
	if (ep == "")
		return getUriEndpoint(uri.substr(0, uri.find_last_of('/'))); // Here we make a substring from the start to '/' (basically with every function call we step back a directory)
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

void ResponseHandler::get()
{
	res_.gheader.date = getCurrentTimeGMT();
	std::string uri = removeDuplicateSlashes(req_.rline.uri);

	std::string ep = getUriEndpoint(uri);
	if (ep == "")
		return setCode("404");

	t_endpoint loc = conf_.getLocation(ep);
	if (!isMethodAllowed(GET, loc.lmethod))
		return setCode("405");

	if (uri == ep)	// If the URI matches with the endpoint then we know it's a directory
		return uriDirResponse(loc, ep);
	prepUriFile(uri, loc);
	return setResponseBody(uri);
}
