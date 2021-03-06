#pragma once

#include "../Config.h"
#include "../SubdatinData.h"
#include "../UserData.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>

#include "PageHeader.h"
#include "PageFooter.h"

void createNewThreadPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createNewThreadPage(FcgiData* fcgi, RequestData* data, std::string error);