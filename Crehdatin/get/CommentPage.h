#pragma once

#include "../Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>

#include "../UserData.h"
#include "../StringHelper.h"
#include "PageHeader.h"
#include "PageFooter.h"
#include "ThreadPage.h"

void createCommentPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
