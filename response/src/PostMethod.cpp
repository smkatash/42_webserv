#include "PostMethod.hpp"
#include <map>
#include <fstream>
#include <string>
#include "Response.hpp" // Status codes definitions
#include "parser_utils.hpp" // Methods enum
#include "response_utils.hpp"
#include "CGIHandler.hpp"

PostMethod::PostMethod(Request& req, ConfigFile& conf)
: ResponseGenerator(req, conf) {}

PostMethod::~PostMethod() {}

std::string PostMethod::findUriEndpoint(const std::string& uri)
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
void PostMethod::prepUriFile(std::string& uri, const t_endpoint& loc)
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

void	PostMethod::setResponseBody(std::string fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open() || !file.good())
		return setCode(NOTFOUND);
	std::string temp;
	while (std::getline(file, temp))
		res_.rbody += temp + '\n';
	res_.eheader.contentLength = toString(res_.rbody.length());
	file.close();
	return setCode(OK);
}

void PostMethod::returnResponse(t_endpoint loc)
{
	res_.rheader.location = loc.lredirect;
	if (loc.lredirect.substr(0, 4) == "http")
		return setCode(FOUND);
	else
		return setCode(MOVEDPERMAN);
}

void PostMethod::dirResponse(t_endpoint loc, std::string ep)
{
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

void PostMethod::normalResponse(t_endpoint loc, std::string uri)
{
	prepUriFile(uri, loc);
	std::string uriPath = uri.substr(0, uri.find('?'));
	res_.eheader.contentType = findContentType(uriPath.substr(uri.find_last_of('.')));
	return setResponseBody(uriPath);
}

void PostMethod::post()
{
	try
	{
		res_.gheader.date = findCurrentTimeGMT();
		std::string uri = removeDuplicateSlashes(req_.rline.uri);
		std::string ep = findUriEndpoint(uri.substr(0, uri.find('?')));
		t_endpoint loc = conf_.getLocation(ep); // try-catch because getLocation may throw an exception

		if (conf_.getClientMaxBodySize() != 0
			&& conf_.getClientMaxBodySize() < strtonum<unsigned long>(req_.eheader.contentLength))
			return setCode(NOTALLOWED);

		if (!isMethodAllowed(POST, loc.lmethod))
			return setCode(NOTALLOWED);
		/* Check if you have to send to cgi handler by checking if
		there's a query */
		if (!req_.rbody.empty())
		{
			CGIHandler cgi(req_, conf_, ep);
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			std::cout << "CGI Response " << res_.cgiResponse << std::endl;
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
		return setCode(NOTFOUND);
	}
}
