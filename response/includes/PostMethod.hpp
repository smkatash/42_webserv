#ifndef POSTMETHOD_HPP
#define POSTMETHOD_HPP

#include "ResponseGenerator.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"

class PostMethod : virtual public ResponseGenerator
{
public:
	PostMethod(Request& req, ConfigFile& conf);
	~PostMethod();

	void	post();
};

#endif
