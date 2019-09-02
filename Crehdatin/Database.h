#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <mysql_driver.h>

#include <iostream>
#include <memory>
#include <boost/filesystem.hpp>

class Database{
public:
	Database() = delete;
	static bool createDatabase();
	static bool deleteDatabase();
	static bool checkDatabase();
};
