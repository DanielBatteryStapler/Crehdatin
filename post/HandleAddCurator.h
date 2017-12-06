#pragma once

#include "../Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>
#include <WebsiteFramework/Response.h>

#include "../RequestData.h"
#include "../UserData.h"
#include "../UserPermissions.h"
#include "../get/PageHeader.h"
#include "../get/PageFooter.h"
#include "../get/ErrorPages.h"

void handleAddCurator(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);