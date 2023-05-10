#include "PostMethod.hpp"

PostMethod::PostMethod(Request& req, ConfigFile& conf)
: ResponseGenerator(req, conf) {}

PostMethod::~PostMethod() {}
