#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>
#include <WebsiteFramework/Response.h>

#include "RequestData.h"
#include "UserPermissions.h"
#include "ErrorPages.h"

void handleRemoveSubdatinOfficial(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);