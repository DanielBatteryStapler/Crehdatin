#pragma once

#include <memory>

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/Response.h>
#include <WebsiteFramework/Cryptography.h>
#include <WebsiteFramework/InputHandler.h>

#include "RequestData.h"
#include "PageHeader.h"
#include "PageFooter.h"
#include "ErrorPages.h"

bool requestStartHandle(FcgiData* fcgi, void* _data);
bool getUserRequestData(FcgiData* fcgi, RequestData* data, std::string sessionToken);
void createNewSession(RequestData* data);