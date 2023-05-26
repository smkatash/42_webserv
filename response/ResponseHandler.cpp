#include <map>
#include <fstream>
#include <string>
#include "Response.hpp" // Status codes definitions
#include "Parser.hpp" // Methods enum
#include "response_utils.hpp"
#include "CGIHandler.hpp"
#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: req_(req)
, conf_(conf)
{
	res_.rline.version = "HTTP/1.1";
	res_.rline.statusCode = "200";
	res_.rline.reasonPhrase = "OK";
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

bool	ResponseHandler::isMethodAllowed(Methods method, std::vector<int> methodsAllowed)
{
	if (methodsAllowed.empty())
		return true;
	std::vector<int>::iterator it = std::find(methodsAllowed.begin(), methodsAllowed.end(), method);
	if (it == methodsAllowed.end())
	{
		setCode(NOTALLOWED);
		return false;
	}
	return true;
}

void	ResponseHandler::setBodyErrorPage(int code)
{
	std::string fileName;
	std::map<int, std::string> errfile = conf_.getErrorFile();
	std::map<int, std::string>::iterator it = errfile.find(code);
	if (it != errfile.end())
		fileName = conf_.getRoot("").substr(0, conf_.getRoot("").find_last_of('/')) + it->second;
	else
		fileName = "./error_pages/" + toString(code) + ".html";

	std::ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open() || !file.good())
		return ;
	std::string temp;
	while (std::getline(file, temp))
	{
		if (file.eof())
			res_.rbody += temp;
		else
			res_.rbody += temp + '\n';
	}
	res_.eheader.contentLength = toString(res_.rbody.length());
	file.close();
}

void	ResponseHandler::setCode(int code)
{
	// TODO: Set different error page from config file
	res_.rline.statusCode = toString(code);
	if (code == OK)
		res_.rline.reasonPhrase = "OK";
	else if (code == FOUND)
		res_.rline.reasonPhrase = "Found";
	else if (code == NOTFOUND)
	{
		res_.rline.reasonPhrase = "Not Found";
		setBodyErrorPage(code);
	}
	else if (code == NOTALLOWED)
		res_.rline.reasonPhrase = "Not Allowed";
	else if (code == INTERNALERROR)
		res_.rline.reasonPhrase = "Internal Server Error";
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
	if (uri[0] == '/')
		uri.erase(uri.begin());
	if (loc.lroot.empty())
		uri.insert(0, conf_.getRoot(""));
	else
		uri.insert(0, loc.lroot);
	if (uri[uri.size() - 1] == '/')
		uri.erase(uri.end());
	if (uri[0] == '/')
		uri.insert(0, ".");
}

void	ResponseHandler::setResponseBody(std::string fileName)
{
	std::ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open())
		return setCode(NOTFOUND);
	if (!file.good())
		return setCode(INTERNALERROR);
	std::string temp;
	while (std::getline(file, temp))
	{
		if (file.eof())
			res_.rbody += temp;
		else
			res_.rbody += temp + '\n';
	}
	res_.eheader.contentLength = toString(res_.rbody.length());
	res_.gheader.connection = "keep-alive";
	file.close();
	return setCode(OK);
}

void ResponseHandler::returnResponse(t_endpoint loc)
{
	res_.rheader.location = loc.lredirect;
	if (loc.lredirect.substr(0, 4) == "http")
		return setCode(FOUND);
	else
		return setCode(MOVEDPERMAN);
}

void ResponseHandler::autoIndexResponse(t_endpoint loc, std::string ep)
{
	std::string templateFile;
	if (loc.lroot.empty())
		templateFile = initAutoIndex(ep, conf_.getRoot(""));
	else
		templateFile = initAutoIndex(ep, loc.lroot);
	if (templateFile.empty())
		return setCode(NOTFOUND);
	res_.rbody = templateFile;
	res_.eheader.contentType = findContentType(".html");
	res_.eheader.contentLength = toString(templateFile.size());
	return setCode(OK);
}

void ResponseHandler::dirResponse(t_endpoint loc, std::string ep)
{
	if (loc.lautoindex)
		return autoIndexResponse(loc, ep);
	if (loc.lindex.empty())
		return setCode(NOTFOUND);
	std::string dir;
	if (loc.lroot.empty())
		dir = conf_.getRoot("") + ep.substr(1);
	else
		dir = loc.lroot + ep.substr(1);
	std::string uri = findUsableFile(loc.lindex, dir);
	if (uri.empty())
		return setCode(NOTFOUND);
	uri = ep + '/' + uri;
	return normalResponse(loc, uri);
}

void ResponseHandler::normalResponse(t_endpoint loc, std::string uri)
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
		std::string ep = findUriEndpoint(uri.substr(0, uri.find('?')));
		t_endpoint loc = conf_.getLocation(ep); // try-catch because getLocation may throw an exception
		if (!isMethodAllowed(GET, loc.lmethod))
			return setCode(NOTALLOWED);
		/* Check if you have to send to cgi handler by checking if
		there's a query */
		if (uri.find('?') != std::string::npos)
		{
			CGIHandler cgi(req_, conf_, ep, uri.substr(uri.find('?') + 1));
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			return ;
		}
		if (!loc.lredirect.empty())
			return returnResponse(loc);
		if (uri == ep)	// If the URI matches with the endpoint then we know it's a directory
			return dirResponse(loc, ep);
		return normalResponse(loc, uri);
	}
	catch(const std::exception& e)
	{
		if (strcmp(e.what(),"endpoint not found") == 0 )
			return setCode(NOTFOUND);
		return setCode(INTERNALERROR);
	}
}

void ResponseHandler::post()
{
	try
	{
		res_.gheader.date = findCurrentTimeGMT();
		std::string uri = removeDuplicateSlashes(req_.rline.uri);
		std::string ep = findUriEndpoint(uri.substr(0, uri.find('?')));
		t_endpoint loc = conf_.getLocation(ep);

		/* TODO: Handle cases where content length isn't known */
		if (conf_.getClientMaxBodySize() != 0
			&& conf_.getClientMaxBodySize() < strtonum<unsigned long>(req_.eheader.contentLength))
			return setCode(NOTALLOWED);
		if (!isMethodAllowed(POST, loc.lmethod))
			return setCode(NOTALLOWED);

		/* check if chunked and dechunk accordingly */
		if (req_.gheader.transferEncoding.compare("chunked") == 0) {
			req_.rbody = unchunkData(req_.rbody);
		}
		/* Check if you have to send to cgi handler by checking if
		there's a query */
		if (!req_.rbody.empty())
		{
			CGIHandler cgi(req_, conf_, ep);
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			// std::cout << "CGI Response " << res_.cgiResponse << std::endl;
			return ;
		}
	}
	catch(const std::exception& e)
	{
		if (strcmp(e.what(), "endpoint not found") == 0)
			return setCode(NOTFOUND);
		return setCode(INTERNALERROR);
	}
}

void ResponseHandler::normalDelResponse(t_endpoint loc, std::string uri)
{
	prepUriFile(uri, loc);
	res_.eheader.contentType = findContentType(uri.substr(uri.find_last_of('.')));
	unlink(uri.c_str());
	return ;
}

void ResponseHandler::dirDelResponse(t_endpoint loc, std::string ep)
{
	if (loc.lautoindex)
		return autoIndexResponse(loc, ep);
	if (loc.lindex.empty())
		return setCode(NOTFOUND);
	std::string dir;
	if (loc.lroot.empty())
		dir = conf_.getRoot("") + ep.substr(1);
	else
		dir = loc.lroot + ep.substr(1);
	std::string uri = findUsableFile(loc.lindex, dir);
	if (uri.empty())
		return setCode(NOTFOUND);
	uri = ep + '/' + uri;
	return normalDelResponse(loc, uri);
}


void ResponseHandler::del()
{
	try
	{
		res_.gheader.date = findCurrentTimeGMT();
		std::string uri = removeDuplicateSlashes(req_.rline.uri);
		std::string ep = findUriEndpoint(uri.substr(0, uri.find('?')));
		t_endpoint loc = conf_.getLocation(ep); // try-catch because getLocation may throw an exception

		if (!isMethodAllowed(DELETE, loc.lmethod))
			return setCode(NOTALLOWED);
		if (uri.find('?') != std::string::npos)
		{
			CGIHandler cgi(req_, conf_, ep, uri.substr(uri.find('?') + 1));
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			return ;
		}
		if (uri == ep)	// If the URI matches with the endpoint then we know it's a directory
			return dirDelResponse(loc, ep);
		return normalDelResponse(loc, uri);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

std::string ResponseHandler::generate()
{
	// if (!res_.cgiResponse.empty())
	// 	return res_.cgiResponse;
	// return "IT IS EMPTY";
	if (!res_.cgiResponse.empty())
		return res_.cgiResponse;
	std::string	header;
	header = responseLine() + generalHeader()
				+ responseHeader() + entityHeader();
	if (!res_.rbody.empty())
		header.append("\r\n" + res_.rbody);
	return header;
}

void	ResponseHandler::handle()
{
	if (req_.rline.method == "GET")
		return get();
	if (req_.rline.method == "POST")
		return post();
	if (req_.rline.method == "DELETE")
		return del();
}
