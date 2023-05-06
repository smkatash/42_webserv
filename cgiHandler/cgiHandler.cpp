#include "cgiHandler.hpp"

CGIHandler::CGIHandler(Request req): req_(req) {
	initialize();
};

CGIHandler::~CGIHandler() {};

void	CGIHandler::setEnvironment() {
	setenv("REQUEST_METHOD", req_.rline.method.c_str(), 1);
	setenv("CONTENT_TYPE", req_.eheader.contentType.c_str(), 1);
	setenv("CONTENT_LENGTH", req_.eheader.contentLength.c_str(), 1);
	setenv("HTTP_USER_AGENT", req_.rheader.userAgent.c_str(), 1);
	setenv("SCRIPT_NAME", scriptPath_.c_str(), 1);
	setenv("QUERY_STRING", queryString_.c_str(), 1);
	setenv("SERVER_NAME", serverName_.c_str(), 1);
}


// TODO serverName form config or passed into constructor?
void	CGIHandler::initialize() {
	cgi_.serverName = conf_.getServerName().c_str();
	cgi_.method = req_.eheader.contentType.c_str();
	cgi_.contenType = req_.eheader.contentType.c_str();
	cgi_.contenLength = req_.eheader.contentLength.c_str();
	cgi_.userAgent = req_.rheader.userAgent.c_str();
	

}