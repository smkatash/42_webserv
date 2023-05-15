#include <map>
#include <fstream>
#include <string>
#include "Response.hpp" // Status codes definitions
#include "GetMethod.hpp"
#include "parser_utils.hpp" // Methods enum
#include "response_utils.hpp"
#include "CGIHandler.hpp"

GetMethod::GetMethod(Request& req, ConfigFile& conf)
: ResponseGenerator(req, conf) {}

GetMethod::~GetMethod() {}

std::string GetMethod::findUriEndpoint(const std::string& uri)
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
void GetMethod::prepUriFile(std::string& uri, const t_endpoint& loc)
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

void	GetMethod::setResponseBody(std::string fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open() || !file.good())
		return setCode(NOTFOUND);
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
	return setCode(OK);
}

void GetMethod::returnResponse(t_endpoint loc)
{
	res_.rheader.location = loc.lredirect;
	if (loc.lredirect.substr(0, 4) == "http")
		return setCode(FOUND);
	else
		return setCode(MOVEDPERMAN);
}

void GetMethod::autoIndexResponse(t_endpoint loc, std::string ep)
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

void GetMethod::dirResponse(t_endpoint loc, std::string ep)
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

void GetMethod::normalResponse(t_endpoint loc, std::string uri)
{
	prepUriFile(uri, loc);
	std::string uriPath = uri.substr(0, uri.find('?'));
	res_.eheader.contentType = findContentType(uriPath.substr(uri.find_last_of('.')));
	return setResponseBody(uriPath);
}

void GetMethod::get()
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
		return setCode(NOTFOUND);
	}
}
