#include "ResponseGenerator.hpp"
#include "GetMethod.hpp"
#include "PostMethod.hpp"
#include "DeleteMethod.hpp"
#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler(Request req, ConfigFile conf)
: ResponseGenerator(req, conf), GetMethod(req, conf), PostMethod(req, conf), DeleteMethod(req, conf) {}

ResponseHandler::~ResponseHandler() {}
