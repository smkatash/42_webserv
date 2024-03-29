#include "RequestParser.hpp"

RequestParser::RequestParser() {
}

RequestParser::~RequestParser() {}


void	RequestParser::getParam_(std::string& str, std::string& dest) {
	std::stringstream	ss(str);
	std::string param;
	ss >> param;
	while (ss >> param) {
		dest += param + " ";
	}
}

void	RequestParser::initParser(std::string input) {
	std::stringstream	ss(input);
	std::string			line;

	while(std::getline(ss, line)) {
		if (!line.empty() && isRequestLine(line))
			parseRequestLine_(line);
		else if (!line.empty() && isGeneralHeader(line))
			parseGeneralHeader_(line);
		else if (!line.empty() && isRequestHeader(line))
			parseRequestHeader_(line);
		else if (!line.empty() && isEntityHeader(line))
			parseEntityHeader_(line);
		else if (line.compare("\r") == 0) {
			std::getline(ss, line);
			if (isBoundary(line, req_.eheader.boundaryName)) {
				while (std::getline(ss, line)) {
					if (line.compare("\r") == 0)
						break;
					parseEntityHeader_(line);
				}
			} else {
				req_.rbody += line;
			}
			while (!ss.eof() && std::getline(ss, line)) {
				if (startsWith(line, "--") && isBoundary(line, req_.eheader.boundaryName))
					break;
				if (!ss.eof())
					line += '\n';
				req_.rbody += line;
			}
			break;
		}
	}
}


void	RequestParser::parseRequestLine_(std::string line) {
	std::stringstream ss(line);
	ss >> req_.rline.method >> req_.rline.uri >> req_.rline.httpVersion;
}

void	RequestParser::parseGeneralHeader_(std::string line) {
	std::stringstream	ss(line);
	std::string			param;

	ss >> param;
	if (param.compare("Cache-Control:") == 0)
		getParam_(line, req_.gheader.cache);
	else if (param.compare("Connection:") == 0)
		getParam_(line, req_.gheader.connection);
	else if (param.compare("Date:") == 0)
		getParam_(line, req_.gheader.date);
	else if (param.compare("Pragma:") == 0)
		getParam_(line, req_.gheader.pragma);
	else if (param.compare("Trailer:") == 0)
		getParam_(line, req_.gheader.trailer);
	else if (param.compare("Transfer-Encoding:") == 0)
		getParam_(line, req_.gheader.transferEncoding);
	else if (param.compare("Upgrade:") == 0)
		getParam_(line, req_.gheader.upgrade);
	else if (param.compare("Via:") == 0)
		getParam_(line, req_.gheader.via);
	else if (param.compare("Warning:") == 0)
		getParam_(line, req_.gheader.warning);
}

void	RequestParser::parseRequestHeader_(std::string line) {
	std::stringstream	ss(line);
	std::string			param;

	ss >> param;
	if (param.compare("Accept:") == 0)
		getParam_(line, req_.rheader.accept);
	else if (param.compare("Accept-Charset:") == 0)
		getParam_(line, req_.rheader.acceptCharset);
	else if (param.compare("Accept-Encoding:") == 0)
		getParam_(line, req_.rheader.acceptEncoding);
	else if (param.compare("Accept-Language:") == 0)
		getParam_(line, req_.rheader.acceptLanguage);
	else if (param.compare("Authorization:") == 0)
		getParam_(line, req_.rheader.authorization);
	else if (param.compare("Expect:") == 0)
		getParam_(line, req_.rheader.expect);
	else if (param.compare("Host:") == 0)
		getParam_(line, req_.rheader.host);
	else if (param.compare("From:") == 0)
		getParam_(line, req_.rheader.from);
	else if (param.compare("If-Match:") == 0)
		getParam_(line, req_.rheader.ifMatch);
	else if (param.compare("If-Modified-Since:") == 0)
		getParam_(line, req_.rheader.ifModified);
	else if (param.compare("If-None-Match:") == 0)
		getParam_(line, req_.rheader.ifNone);
	else if (param.compare("If-Range:") == 0)
		getParam_(line, req_.rheader.ifRange);
	else if (param.compare("If-Unmodified-Since:") == 0)
		getParam_(line, req_.rheader.ifUnmodified);
	else if (param.compare("Max-Forwards:") == 0)
		getParam_(line, req_.rheader.maxForwards);
	else if (param.compare("Proxy-Authorization:") == 0)
		getParam_(line, req_.rheader.proxy_auth);
	else if (param.compare("Range:") == 0)
		getParam_(line, req_.rheader.range);
	else if (param.compare("Cookie:") == 0)
		getParam_(line, req_.rheader.cookie);
	else if (param.compare("Referer:") == 0)
		getParam_(line, req_.rheader.referer);
	else if (param.compare("TE:") == 0)
		getParam_(line, req_.rheader.te);
	else if (param.compare("User-Agent:") == 0)
		getParam_(line, req_.rheader.userAgent);
}

void	RequestParser::parseEntityHeader_(std::string line) {
	std::stringstream	ss(line);
	std::string			param;

	ss >> param;
	if (param.compare("Allow:") == 0)
		getParam_(line, req_.eheader.allow);
	else if (param.compare("Content-Encoding:") == 0)
		getParam_(line, req_.eheader.contentEncoding);
	else if (param.compare("Content-Language:") == 0)
		getParam_(line, req_.eheader.contentLanguage);
	else if (param.compare("Content-Length:") == 0)
		getParam_(line, req_.eheader.contentLength);
	else if (param.compare("Content-Location:") == 0)
		getParam_(line, req_.eheader.contentLocation);
	else if (param.compare("Content-MD5:") == 0)
		getParam_(line, req_.eheader.contentMd);
	else if (param.compare("Content-Range:") == 0)
		getParam_(line, req_.eheader.contentRange);
	else if (param.compare("Content-Type:") == 0)
		parseMultiParamContent(line);
	else if (param.compare("Content-Disposition:") == 0) 
		parseContentDisposition(line);
	else if (param.compare("Expires:") == 0)
		getParam_(line, req_.eheader.expires);
	else if (param.compare("Last-Modified:") == 0)
		getParam_(line, req_.eheader.lastModified);
}

void	RequestParser::parseContentDisposition(std::string line) {
		if (req_.eheader.contentDisposition.empty()) {
			getParam_(line, req_.eheader.contentDisposition);
			std::size_t filenamePos = line.find("filename=\"");
			if (filenamePos != std::string::npos) {
				filenamePos += 10;
				std::size_t endPos = line.find('"', filenamePos);
				if (endPos != std::string::npos) {
					req_.eheader.fileName = line.substr(filenamePos, endPos - filenamePos);
				}
			}
		}
}

void	RequestParser::parseMultiParamContent(std::string line) {
	if (req_.eheader.contentType.empty()) {
		if (line.find("multipart/form-data;") != std::string::npos && \
				line.find(BOUNDARY) != std::string::npos ) {
			size_t delimiterPos = line.rfind(EQ);
			if (delimiterPos != std::string::npos) {
				std::string boundary = line.substr(delimiterPos + 1);
				boundary.erase(std::remove(boundary.begin(), boundary.end(), '-'), boundary.end());
				req_.eheader.boundaryName = boundary;
				req_.eheader.contentType = MULTIPART;
			}
		}
		else
			getParam_(line, req_.eheader.contentType);
	}
	else {
		getParam_(line, req_.eheader.fileContentType);
	}
}


Request	RequestParser::getRequest() {
	return req_;
}

void	RequestParser::debug() {
	std::cout << req_.rline << std::endl;
	std::cout << req_.gheader << std::endl;
	std::cout << req_.rheader << std::endl;
	std::cout << req_.eheader << std::endl;
	
	std::cout << "Here comes the body" << std::endl;
	std::cout << req_.rbody << std::endl;
}

