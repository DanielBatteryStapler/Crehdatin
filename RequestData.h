#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <WebsiteFramework/WebsiteFramework.h>

struct RequestData{
	sql::Connection* con;
	sql::Statement* stmt;
	
	//user data
	
	std::string sessionToken;
	std::string userName;
	int64_t userId;
	std::string authToken;
	std::string shownId;
	std::string cssTheme;
	bool blocked;
	int64_t lastPostTime;
	
	//request data
	
	int64_t currentTime;
	int64_t subdatinId;
	int64_t threadId;
	int64_t commentId;
};
