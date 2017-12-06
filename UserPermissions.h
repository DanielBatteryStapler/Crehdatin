#pragma once

#include <memory>

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include "UserData.h"

enum class UserPosition{None, Error, Curator, Bureaucrat, Administrator, Senate};

UserPosition getEffectiveUserPosition(sql::Connection* con, int64_t userId, int64_t subdatinId = -1);
bool hasRainbowTextPermissions(UserPosition position);
bool hasModerationPermissions(UserPosition position);
bool hasSubdatinControlPermissions(UserPosition position);
bool hasAdministrationControlPermissions(UserPosition position);