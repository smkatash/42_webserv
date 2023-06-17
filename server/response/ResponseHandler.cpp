#include <map>
#include <fstream>
#include <string>
#include "Response.hpp" // Status codes definitions
#include "Parser.hpp" // Methods enum
#include "responseUtils.hpp"
#include "CGIHandler.hpp"
#include "ResponseHandler.hpp"

#define COOKIES

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: req_(req)
, conf_(conf)
{
	/* basic setup for creating a response */
	res_.rline.version      = HTTPVERSION;
	res_.rline.statusCode   = "200";
	res_.rline.reasonPhrase = "OK";
	res_.rheader.server     = conf_.getServerName().empty() ? "Francesco's Pizzeria/2.0 (MacOS)" : conf_.getServerName(); // TODO: Discuss if you keep this or no
	res_.gheader.date       = findCurrentTimeGMT();
	if (!checkRequest())
		return;
	try
	{
		uri_      = removeDuplicateSlashes(req_.rline.uri);
		endpoint_ = findUriEndpoint(uri_.substr(0, uri_.find('?')));
		location_ = conf_.getLocation(endpoint_);
	}
	catch(const std::exception& e)
	{
		res_.gheader.connection = "close";
		if (strcmp(e.what(),"endpoint not found") == 0 )
			setCode(NOTFOUND);
		else
			setCode(INTERNALERROR);
		return;
	}
	if (!checkMethod())
		return;
#ifdef COOKIES
	if (!authenticated())
		authenticate();
#endif
}

ResponseHandler::~ResponseHandler() {}


/* Methods ---------------------------------------------------------------------------------------------- */

void ResponseHandler::get()
{
	/* Check if you have to send to cgi handler by checking if
	there's a query and if there's cgi in config file */
	if (!location_.lcgi.second.empty() && uri_.find('?') != std::string::npos)
	{
		CGIHandler cgi(req_, conf_, endpoint_, uri_.substr(uri_.find('?') + 1));
		cgi.execute();
		res_.cgiResponse = cgi.getCGIResponse();
		processCGIResponse(res_.cgiResponse);
		return ;
	}
	/* if there's no cgi or query for cgi we do normal get */
	if (!location_.lredirect.empty())
		return returnResponse();
	if (uri_ == endpoint_)	// If the URI matches with the endpoint then we know it's a directory
		return dirResponse(GET);
	return normalResponse(GET);
}

void ResponseHandler::post()
{
	if (req_.rheader.expect.compare(0, 12, "100-continue") == 0)
		return setCode(CONTINUE);
	if (req_.eheader.contentLength.empty())
	{
		if (req_.gheader.transferEncoding.compare(0, 7, "chunked") != 0) // If there's no chunked encoding then we need length
			return setCode(LENGTHPLS);
		// Otherwise, there's chunked encoding but no content length (meaning the chunks haven't been fully recieved)
		return setCode(ACCEPTED);
	}
	size_t maxBodySize = conf_.getClientMaxBodySize();
	size_t reqContentLength = strtonum<unsigned long>(req_.eheader.contentLength);
	if (maxBodySize != 0 && maxBodySize < reqContentLength)
		return setCode(TOOLARGE);

	/* Check if you have to send to cgi handler by checking if
	there's a body and if there's cgi in config file */
	if (!req_.rbody.empty() && !location_.lcgi.second.empty())
	{
		CGIHandler cgi(req_, conf_, endpoint_);
		cgi.execute();
		res_.cgiResponse = cgi.getCGIResponse();
		processCGIResponse(res_.cgiResponse);
		return ;
	}
	/* if there's no cgi we do normal get */
	return get();
}

void ResponseHandler::del()
{
	if (uri_ == endpoint_)	// If the URI matches with the endpoint then we know it's a directory
		return dirResponse(DELETE);
	return normalResponse(DELETE);
}

void ResponseHandler::handle()
{
	try
	{
		if (res_.rline.statusCode != "200") // Probably endpoint not found, or error in constructor
			return;
		if (req_.rline.method == "GET")
			return get();
		if (req_.rline.method == "POST")
			return post();
		if (req_.rline.method == "DELETE")
			return del();
	}
	catch(const std::exception& e)
	{
		return setCode(INTERNALERROR);
	}
	// Otherwise method not implemented
	return setCode(UNIMPLEMENTED);
}


/* Response Handling ------------------------------------------------------------------------------------ */

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
void ResponseHandler::prepUriFile()
{
	if (uri_[0] == '/')
		uri_.erase(uri_.begin());
	if (location_.lroot.empty())
		uri_.insert(0, conf_.getRoot(""));
	else
		uri_.insert(0, location_.lroot);
	if (uri_[uri_.size() - 1] == '/')
		uri_.erase(uri_.end());
	if (uri_[0] == '/')
		uri_.insert(0, ".");
}

void ResponseHandler::setResponseBody(std::string fileName)
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

	file.close();
	return setCode(OK);
}

/* In case there's redirection defined in config file */
void ResponseHandler::returnResponse()
{
	res_.rheader.location = location_.lredirect;
	res_.rbody = "\r\n";
	if (location_.lredirect.substr(0, 4) == "http")
		return setCode(FOUND);
	else
		return setCode(MOVEDPERMAN);
}

/* In case there's autoindexing in config file */
void ResponseHandler::autoIndexResponse(t_endpoint loc, std::string ep)
{
	std::string templateFile;
	try {
		if (loc.lroot.empty())
			templateFile = initAutoIndex(ep, conf_.getRoot(""));
		else
			templateFile = initAutoIndex(ep, loc.lroot);
	} catch (std::runtime_error& err) {
		return setCode(INTERNALERROR);
	}
	if (templateFile.empty())
		return setCode(NOTFOUND);
	res_.rbody = templateFile;
	res_.eheader.contentType = findContentType(".html");
	res_.eheader.contentLength = toString(templateFile.size());
	return setCode(OK);
}

/* In case the URI is a directory we find the index file inside that directory */
void ResponseHandler::dirResponse(Methods method)
{
	if (location_.lautoindex)
		return autoIndexResponse(location_, endpoint_);
	if (location_.lindex.empty())
		return setCode(NOTFOUND);

	std::string dir;
	if (location_.lroot.empty())
		dir = conf_.getRoot("") + endpoint_.substr(1);
	else
		dir = location_.lroot + endpoint_.substr(1);

	uri_ = findUsableFile(location_.lindex, dir);
	if (uri_.empty())
		return setCode(NOTFOUND);
	uri_ = endpoint_ + '/' + uri_;
	return normalResponse(method);
}

/* For a normal GET or DELETE request (i.e. URI is not a directory) */
void ResponseHandler::normalResponse(Methods method)
{
	prepUriFile();
	std::string uriPath = uri_.substr(0, uri_.find('?'));
	res_.eheader.contentType = findContentType(uriPath.substr(uri_.find_last_of('.')));
	if (method == GET)
		return setResponseBody(uriPath);
	/* else the method is DELETE */
	unlink(uri_.c_str());
	setCode(NOCONTENT);
}

/* Take response from CGI and parse the necessary values from it */
void ResponseHandler::processCGIResponse(std::string& cgi)
{
	std::cout << cgi << std::endl;
	addContentLength(cgi);
	std::istringstream iss(cgi);
	std::string buffer;
	iss >> buffer;
	if (buffer.compare(0, 4, "HTTP") == 0) // There's nothing to change
		return;

	size_t statusLocation = cgi.find("Status: ");
	if (statusLocation == std::string::npos) // Extra safety
		return ;

	std::string status = cgi.substr(statusLocation + 8);
	if (status.compare(0, 2, "OK") == 0)
		setCode(OK);
	else
		setCode(strtonum<int>(status));
	if (strtonum<int>(status) >= 400)
	{
		cgi.clear();
		return;
	}
	cgi = cgi.erase(statusLocation, cgi.find('\n', statusLocation) + 1);
	std::string rline = res_.rline.version + ' ' + res_.rline.statusCode + ' ' + res_.rline.reasonPhrase + "\r\n";
	cgi.insert(0, rline);
}


/* Response Code Setting -------------------------------------------------------------------------------- */

void ResponseHandler::setBodyErrorPage(int code)
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
		fileName = "server/error_pages/" + toString(code) + ".html";
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
	file.close();
	res_.eheader.contentLength = toString(res_.rbody.length());
	res_.eheader.contentType = findContentType(".html");
}

void ResponseHandler::setCode(int code)
{
	res_.rline.statusCode = toString(code);

	size_t numCodes = sizeof(rc) / sizeof(rc[0]);
	for (size_t i = 0; i < numCodes; i++)
	{
		if (rc[i].statusCode == code)
		{
			res_.rline.reasonPhrase = rc[i].reasonPhrase;
			if (rc[i].withBody == true)
				setBodyErrorPage(code);
			break;
		}
	}
}


/* Session Handling ------------------------------------------------------------------------------------- */

void ResponseHandler::addToSessionIds(const std::string& id)
{
	std::ofstream sessionIds;
	sessionIds.open(SIDPATH, std::ios::out | std::ios::app);
	if (sessionIds.fail())
		throw std::ios_base::failure(std::strerror(errno));
	sessionIds.exceptions(sessionIds.exceptions() | std::ios::failbit | std::ifstream::badbit);
	sessionIds << id << std::endl;
	sessionIds.close();
}

bool ResponseHandler::authorized(std::string authorization)
{
	if (authorization.empty())
		return false;

	std::string htPassFileName = location_.lauth_basic_user_file; // parse the auth_basic_user_file
	if (htPassFileName.front() == '/')
		htPassFileName.erase(0, 1);

	std::string filename = "." + location_.lroot + htPassFileName;
	std::ifstream htpassFile(filename);
	if (!htpassFile.is_open() || htpassFile.bad())
		exit(EXIT_FAILURE);
	std::string auth;
	try {
		auth = base64Decode(&authorization[6]); // from 6 because I want to skip "Basic "
	}
	catch(const std::exception& e) {
		return false;
	}
	
	std::string buffer;
	while (std::getline(htpassFile, buffer))
	{
		if (buffer == auth)
		{
			std::string id = get_uuid();
			addToSessionIds(id);
			res_.rheader.setCookie = "session_id=" + id;
			return true;
		}
	}
	return false;
}

bool ResponseHandler::validCookie()
{
	if (req_.rheader.cookie.empty())
		return false;
	std::string cookie = req_.rheader.cookie.substr(req_.rheader.cookie.find("=") + 1);
	std::ifstream sessionIds(SIDPATH);
	std::string	buffer;
	while (std::getline(sessionIds, buffer))
	{
		if (cookie.compare(0, buffer.size(), buffer) == 0)
			return true;
	}
	return false;
}

bool ResponseHandler::authenticated()
{
	if (location_.lauth_basic.empty())
		return true;
	if (validCookie() || authorized(req_.rheader.authorization))
		return true;
	return false;
}

void ResponseHandler::authenticate()
{
	res_.rheader.wwwAuth = "Basic realm=\"" + conf_.getAuthBasic(endpoint_) + "\"";
	res_.eheader.contentLength = "0";
	setCode(UNAUTHORIZED);
}


/* Checks ----------------------------------------------------------------------------------------------- */

bool ResponseHandler::checkRequest()
{
	if (req_.rline.method.empty() || req_.rline.uri.empty() || req_.rline.httpVersion.empty())
		return (setCode(BADREQ), false);
	if (req_.rline.uri.front() != '/')
		return (setCode(BADREQ), false);
	if (req_.rline.uri.size() > 2048)
		return (setCode(LONGURI), false);
	if (req_.rline.httpVersion != HTTPVERSION)
		return (setCode(HTTPNONO), false);
	if (req_.gheader.connection.compare(0, 5, "close") == 0)
		res_.gheader.connection = "close";

	return true;
}

bool ResponseHandler::checkMethod()
{
	size_t i = 0;
	for(; i < 3; i++)
	{
		if (m[i].methodStr == req_.rline.method)
			break;
	}
	if (i == 3)
		return (setCode(UNIMPLEMENTED), false);
	if (!isMethodAllowed(m[i].methodVal))
		return (setCode(NOTALLOWED), false);
	return true;
}

bool ResponseHandler::isMethodAllowed(Methods method)
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


/* Response Generation ---------------------------------------------------------------------------------- */

std::string ResponseHandler::generate()
{
	if (!res_.cgiResponse.empty())
		return res_.cgiResponse;
	std::string	response;
	response = responseLine() + generalHeader()
				+ responseHeader() + entityHeader() + "\r\n";
	if (!res_.rbody.empty())
		response.append(res_.rbody);
	return response;
}

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
