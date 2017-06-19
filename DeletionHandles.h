#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <memory>

void deleteSubdatin(sql::Connection* con, std::size_t id);
void deleteThread(sql::Connection* con, std::size_t id);
void deleteComment(sql::Connection* con, std::size_t id);
void deleteImages(sql::Connection* con, std::size_t threadId, std::size_t commentId);
void deleteImage(sql::Connection* con, std::size_t id);
