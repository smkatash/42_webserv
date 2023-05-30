#include <map>
#include <fstream>
#include <string>
#include <random>
#include "Response.hpp" // Status codes definitions
#include "Parser.hpp" // Methods enum
#include "response_utils.hpp"
#include "CGIHandler.hpp"
#include "ResponseHandler.hpp"

#define COOKIES

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: req_(req)
, conf_(conf)
{
	res_.rline.version = "HTTP/1.1";
	res_.rline.statusCode = "200";
	res_.rline.reasonPhrase = "OK";
	res_.rheader.server = "Francesco's Pizzeria/2.0 (MacOS)";
	try
	{
		res_.gheader.date = findCurrentTimeGMT();
		uri_ = removeDuplicateSlashes(req_.rline.uri);
		endpoint_ = findUriEndpoint(uri_.substr(0, uri_.find('?')));
		location_ = conf_.getLocation(endpoint_); // try-catch because getLocation may throw an exception
#ifdef COOKIES
		if (!authenticated())
			authenticate();
#endif
	}
	catch(const std::exception& e)
	{
		res_.gheader.connection = "close";
		if (strcmp(e.what(),"endpoint not found") == 0 )
			setCode(NOTFOUND);
		else
			setCode(INTERNALERROR);
	}
	
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
		line.append("\r\nCache-Control: " + res_.gheader.cache);
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
	if (!res_.rheader.setCookie.empty())
		line.append("Set-Cookie: " + res_.rheader.setCookie + "\r\n");
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

bool	ResponseHandler::isMethodAllowed(Methods method)
{
	if (location_.lmethod.empty())
		return true;
	std::vector<int>::iterator it = std::find(location_.lmethod.begin(), location_.lmethod.end(), method);
	if (it == location_.lmethod.end())
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
	{
		fileName = conf_.getRoot("").substr(0, conf_.getRoot("").find_last_of('/')) + it->second;
		fileName.insert(fileName.begin(), '.');
	}
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
	if (code == NOCONTENT)
		res_.rline.reasonPhrase = "No Content";
	else if (code == FOUND)
		res_.rline.reasonPhrase = "Found";
	else if (code == UNAUTHORIZED)
		res_.rline.reasonPhrase = "Unauthorized";
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
	res_.rbody = "\r\n";
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
		if (!isMethodAllowed(GET))
			return setCode(NOTALLOWED);
		/* Check if you have to send to cgi handler by checking if
		there's a query */
		if (uri_.find('?') != std::string::npos)
		{
			CGIHandler cgi(req_, conf_, endpoint_, uri_.substr(uri_.find('?') + 1));
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			return ;
		}
		if (!location_.lredirect.empty())
			return returnResponse(location_);
		if (uri_ == endpoint_)	// If the URI matches with the endpoint then we know it's a directory
			return dirResponse(location_, endpoint_);
		return normalResponse(location_, uri_);
	}
	catch(const std::exception& e)
	{
		return setCode(INTERNALERROR);
	}
}

void ResponseHandler::post()
{
	try
	{
		/* TODO: Handle cases where content length isn't known */
		if (conf_.getClientMaxBodySize() != 0
			&& conf_.getClientMaxBodySize() < strtonum<unsigned long>(req_.eheader.contentLength))
			return setCode(NOTALLOWED);
		if (!isMethodAllowed(POST))
			return setCode(NOTALLOWED);

		/* check if chunked and dechunk accordingly */
		if (req_.gheader.transferEncoding.compare("chunked") == 0) {
			req_.rbody = unchunkData(req_.rbody);
		}
		/* Check if you have to send to cgi handler by checking if
		there's a query */
		if (!req_.rbody.empty())
		{
			CGIHandler cgi(req_, conf_, endpoint_);
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			// std::cout << "CGI Response " << res_.cgiResponse << std::endl;
			return ;
		}
	}
	catch(const std::exception& e)
	{
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
		if (!isMethodAllowed(DELETE))
			return setCode(NOTALLOWED);
		if (uri_.find('?') != std::string::npos)
		{
			CGIHandler cgi(req_, conf_, endpoint_, uri_.substr(uri_.find('?') + 1));
			cgi.execute();
			res_.cgiResponse = cgi.getCGIResponse();
			return ;
		}
		if (uri_ == endpoint_)	// If the URI matches with the endpoint then we know it's a directory
			return dirDelResponse(location_, endpoint_);
		return normalDelResponse(location_, uri_);
	}
	catch(const std::exception& e)
	{
		return setCode(INTERNALERROR);
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

std::string get_uuid()
{
	static std::random_device dev;
	static std::mt19937 rng(dev());

	std::uniform_int_distribution<int> dist(0, 15);

	const char *v = "0123456789abcdef";
	const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

	std::string res;
	for (int i = 0; i < 16; i++) {
		if (dash[i]) res += "-";
		res += v[dist(rng)];
		res += v[dist(rng)];
	}
	return res;
}

void	ResponseHandler::addToSessionIds(std::string id)
{
	std::ofstream sessionIds;
	sessionIds.open("./var/www/pages/documents/session_ids", std::ios::out | std::ios::app);
	if (sessionIds.fail())
		throw std::ios_base::failure(std::strerror(errno));
	sessionIds.exceptions(sessionIds.exceptions() | std::ios::failbit | std::ifstream::badbit);
	sessionIds << id << std::endl;
	sessionIds.close();
}

bool	ResponseHandler::authorized(std::string authorization)
{
	if (endpoint_ != "/delete") // Instead of this I should check if there's the auth_basic in config file in this location
		return true;
	if (authorization.empty())
		return false;
	std::string auth = base64Decode(&authorization[6]);

	std::string htPassFileName = conf_.getAuthBasicUserFile(endpoint_);
	if (htPassFileName.front() == '/')
		htPassFileName.erase(0, 1);

	std::string filename = "." + location_.lroot + htPassFileName;

	std::cout << filename << std::endl; // Instead of documents/.htpassword I should take the value from the config file
	std::cout << auth << std::endl;

	// std::ifstream htpassFile(location_.lroot + conf_.getAuthFile());
	std::ifstream htpassFile(filename);
	if (!htpassFile.is_open())
		exit(EXIT_FAILURE);
	if (htpassFile.bad())
		exit(EXIT_FAILURE);
	std::string buffer;
	while (std::getline(htpassFile, buffer))
	{
		std::cout << buffer << std::endl;
		if (buffer == auth)
		{
			std::string id = get_uuid();
			addToSessionIds(id);
			res_.rheader.setCookie = "session_id=" + id + "; path=/delete";
			return true;
		}
	} 
	return false;
}

bool	ResponseHandler::validCookie()
{
	if (req_.rheader.cookie.empty())
		return false;
	std::string cookie = req_.rheader.cookie.substr(req_.rheader.cookie.find("=") + 1);
	std::ifstream sessionIds("./var/www/pages/documents/session_ids");
	std::string	buffer;
	while (std::getline(sessionIds, buffer))
	{
		if (std::strncmp(cookie.c_str(), buffer.c_str(), buffer.size()) == 0)
			return true;
	}
	return false;
}

bool	ResponseHandler::authenticated()
{
	if (validCookie() || authorized(req_.rheader.authorization))
		return true;
	return false;
}

void	ResponseHandler::authenticate()
{
	res_.rheader.wwwAuth = "Basic realm=\"" + conf_.getAuthBasic(endpoint_) + "\"";
	res_.rbody = "\r\n";
	setCode(UNAUTHORIZED);
}

void	ResponseHandler::handle()
{
	if (res_.rline.statusCode != "200") // Probably endpoint not found, or error in constructor
		return;
	if (req_.rline.method == "GET")
		return get();
	if (req_.rline.method == "POST")
		return post();
	if (req_.rline.method == "DELETE")
		return del();
	// otherwise send unhandled method
}
