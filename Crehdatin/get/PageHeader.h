#pragma once

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/Response.h>
#include <WebsiteFramework/InputHandler.h>

#include "../RequestData.h"
#include "../SubdatinData.h"
#include "../UserData.h"
#include "../UserPermissions.h"

enum PageTab{None, Main, User, Login, Logout, Settings, SiteControl, Error, ThreadList, Thread, Comment, About, NewThread, Reports, ControlPanel};

void createPageHeader(FcgiData* fcgi, RequestData* data, PageTab selectedTab);
std::string getPageTitle(FcgiData* fcgi, RequestData* data, PageTab selectedTab);