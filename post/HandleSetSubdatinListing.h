#pragma once

#include <memory>

#include "../Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>

#include "../get/SettingsPage.h"
#include "../SubdatinData.h"

void handleSetSubdatinListing(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void simplifySubdatinListing(sql::Connection* con, std::size_t userId);