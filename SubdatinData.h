#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>
#include <memory>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>

#include "RequestData.h"

int64_t getSubdatinId(sql::Connection* con, std::string subdatinTitle);
void getSubdatinData(sql::Connection* con, int64_t id, std::string& title, std::string& name, bool& postLocked, bool& commentLocked);
void getSubdatinData(sql::Connection* con, int64_t id, std::string& title, std::string& name);
void getSubdatinLockedData(sql::Connection* con, int64_t id, bool& postLocked, bool& commentLocked);
std::string getSubdatinTitle(sql::Connection* con, int64_t subdatinId);
int64_t getThreadCommentCount(sql::Connection* con, int64_t threadId);
MarkupString getFormattedTime(sql::Connection* con, int64_t time, std::string verb);
MarkupString getFormattedThreadPostTime(sql::Connection* con, int64_t threadId);
MarkupString getFormattedThreadBumpTime(sql::Connection* con, int64_t threadId);
MarkupString getFormattedCommentPostTime(sql::Connection* con, int64_t commentId);
int64_t getParentComment(sql::Connection* con, int64_t commentId);
