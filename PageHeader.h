#pragma once

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/Response.h>
#include <WebsiteFramework/InputHandler.h>

#include "RequestData.h"
#include "SubdatinData.h"
#include "UserData.h"
#include "UserPermissions.h"

void createPageHeader(FcgiData* fcgi, RequestData* data);