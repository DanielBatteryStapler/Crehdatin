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
#include "get/PageHeader.h"
#include "get/PageFooter.h"
#include "get/ErrorPages.h"

#include "Captcha.h"

bool requestStartHandle(FcgiData* fcgi, void* _data);
bool getSessionData(FcgiData* fcgi, RequestData* data, std::string sessionToken);
void createNewSession(FcgiData* fcgi, RequestData* data);