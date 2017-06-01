#pragma once

#include <memory>


#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>

#include "Config.h"
#include "UserPermissions.h"
#include "RequestData.h"

std::string getUserName(sql::Connection* con, int64_t userId);
int64_t getUserId(sql::Connection* con, std::string userName);
void setLastPostTime(FcgiData* fcgi, RequestData* data);
std::string getFormattedPosterString(sql::Connection* con, std::string anonId, int64_t userId, int64_t subdatinId = -1);
std::string getEffectiveUserPosition(sql::Connection* con, int64_t userId, int64_t subdatinId = -1);