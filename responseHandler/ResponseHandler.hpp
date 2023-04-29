#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include <iostream>
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"

class ResponseHandler {
private:
	Response	res_;
	Request		req_;
	ConfigFile	conf_;

	std::string	appendResponseLine();
	std::string	appendGeneralHeader();
	std::string	appendResponseHeader();
	std::string	appendEntityHeader();
public:
	ResponseHandler(Request req, ConfigFile conf);
	~ResponseHandler();

	std::string	getResponse();

	void		getMethod();
	void		postMethod();
	void		deleteMethod();
	void		headMethod();
};

#endif
