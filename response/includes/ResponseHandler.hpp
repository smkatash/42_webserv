#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "GetMethod.hpp"
#include "PostMethod.hpp"
#include "DeleteMethod.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"

class ResponseHandler : public GetMethod, public PostMethod, public DeleteMethod
{
public:
	ResponseHandler(Request req, ConfigFile conf);
	~ResponseHandler();
};

#endif
