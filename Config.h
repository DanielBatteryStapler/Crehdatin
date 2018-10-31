#pragma once

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

class Config{
public:
	Config() = delete;
	static std::string getSqlAddress();
	static std::string getSqlUserName();
	static std::string getSqlPassword();
	static std::string getSqlDatabaseName();
	static boost::filesystem::path getCrehdatinDataDirectory();
	static std::size_t getUniqueTokenLength();
	static std::size_t getMaxNameLength();
	static std::size_t getMaxPasswordLength();
	static std::size_t getMaxPostLength();
	static std::size_t getMaxTitleLength();
	static std::size_t getMaxPostImageSize();
	static int64_t anonPostingTimeout();
	static int64_t userPostingTimeout();
	static int64_t anonReportingTimeout();
	static int64_t userReportingTimeout();
	static std::size_t getMaxReportLength();
	static std::size_t getMinUserNameLength();
};

