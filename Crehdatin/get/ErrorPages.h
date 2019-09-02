#pragma once

#include <memory>

#include "../Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>
#include "PageHeader.h"
#include "PageFooter.h"

void createGenericErrorPage(FcgiData* fcgi, RequestData* data, std::string message);
void createInvalidPermissionsErrorPage(FcgiData* fcgi, RequestData* data);
void createMustBeLoggedInErrorPage(FcgiData* fcgi, RequestData* data);