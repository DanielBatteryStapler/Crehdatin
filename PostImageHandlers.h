#pragma once

#include "Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <cgicc/Cgicc.h>
#include <cgicc/FormFile.h>
#include <Magick++.h>
#include <atomic>
#include <chrono>

#include <WebsiteFramework/Cryptography.h>

struct UploadedImageData{
	std::string originalFileName;
	Magick::Image image;
};

enum class PostImageError{NoError = 0, NoFile, InvalidType, IsTooLarge};

PostImageError getUploadedPostImage(cgicc::Cgicc* cgi, UploadedImageData& imageData, std::string name, std::size_t maximumSize);
void saveUploadedPostImage(sql::Connection* con, UploadedImageData&& imageData, bool showThumbnail, std::size_t threadId, std::size_t commentId);
