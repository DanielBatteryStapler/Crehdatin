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

#include "../get/CrehdatinControlPanelPage.h"
#include "../SubdatinData.h"
#include "HandleSetSubdatinListing.h"

void handleSetDefaultSubdatinListing(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);