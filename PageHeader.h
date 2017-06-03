#pragma once

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/Response.h>
#include <WebsiteFramework/InputHandler.h>

#include "RequestData.h"
#include "SubdatinData.h"
#include "UserData.h"
#include "UserPermissions.h"

enum PageTab{None, Main, Login, Settings, SiteControl, Error, ThreadList, Thread, About, NewThread, Reports, ControlPanel};

void createPageHeader(FcgiData* fcgi, RequestData* data, PageTab selectedTab);