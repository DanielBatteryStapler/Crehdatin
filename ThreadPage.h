#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>

#include "UserData.h"
#include "StringHelper.h"
#include "PageHeader.h"
#include "PageFooter.h"
#include "FormatPost.h"

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createCommentLine(std::ostream& fcgiOut, RequestData* data, bool canModerate, bool canReply, std::string& subdatinTitle, int64_t layer = 0, int64_t parentId = -1);

bool renderThread(std::ostream& fcgiOut, RequestData* data, std::int64_t subdatinId, std::string& subdatinTitle, std::size_t threadId, bool isPreview, bool canModerate = false, bool canControl = false, bool showSubdatin = false);
//the returned bool is if you can reply to this thread or not
void renderComment(std::ostream& fcgiOut, RequestData* data, std::int64_t subdatinId, std::string& subdatinTitle, std::size_t commentId, bool isEven, bool isPreview, bool showPoster, bool showPermaLink = true, bool canReply = false, bool canModerate = false, bool showSubdatin = false);

void createReportMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t commentId = -1);
void createReplyMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t commentId = -1);
void createThreadModerationMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, bool canControl, bool locked = false, bool stickied = false);
void createCommentModerationMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, int64_t commentId);