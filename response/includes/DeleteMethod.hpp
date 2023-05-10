#ifndef DELETEMETHOD_HPP
#define DELETEMETHOD_HPP

#include "ResponseGenerator.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"

class DeleteMethod : virtual public ResponseGenerator
{
public:
	DeleteMethod(Request& req, ConfigFile& conf);
	~DeleteMethod();

	void		del();
};

#endif
