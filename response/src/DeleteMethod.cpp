#include "DeleteMethod.hpp"

DeleteMethod::DeleteMethod(Request& req, ConfigFile& conf)
: ResponseGenerator(req, conf) {}

DeleteMethod::~DeleteMethod() {}
