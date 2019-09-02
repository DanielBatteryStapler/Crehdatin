#include "Config.h"

std::string Config::getSqlAddress(){
	return "127.0.0.1:3306";
}

std::string Config::getSqlUserName(){
	return "Crehdatin";
}

std::string Config::getSqlPassword(){
	/*
	static std::string password;
	if(password.size() == 0){
		std::cout << "Input SQL password for user \"" << getSqlUserName() << "\":";
		std::getline(std::cin, password);
	}
	return password;
	*/
	return "password";
}

std::string Config::getSqlDatabaseName(){
	return "Crehdatin";
}

boost::filesystem::path Config::getCrehdatinDataDirectory(){
	#ifdef DEBUG
	return boost::filesystem::path("/home/daniel/Projects/crehdatinData");
	#else
	return boost::filesystem::path("/home/danielbatterystapler/webServer/crehdatinData");
	#endif // DEBUG
}

std::size_t Config::getUniqueTokenLength(){
	return 512;
}

std::size_t Config::getMaxNameLength(){
	return 64;
}

std::size_t Config::getMaxPasswordLength(){
	return 512;
}

std::size_t Config::getMaxPostLength(){
	return 65535;
}

std::size_t Config::getMaxTitleLength(){
	return 64;
}

std::size_t Config::getMaxPostImageSize(){
	return 4000000;//4MB
}

int64_t Config::anonPostingTimeout(){
	return 15;
}

int64_t Config::userPostingTimeout(){
	return 15;
}

int64_t Config::anonReportingTimeout(){
	return 15;
}

int64_t Config::userReportingTimeout(){
	return 15;
}

std::size_t Config::getMaxReportLength(){
	return 100;
}

std::size_t Config::getMinUserNameLength(){
	return 3;
}
