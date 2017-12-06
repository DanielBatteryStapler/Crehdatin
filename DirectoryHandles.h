#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>

#include "get/PageHeader.h"
#include "get/PageFooter.h"

bool subdatinDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
bool threadDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
bool commentDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
bool userDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
