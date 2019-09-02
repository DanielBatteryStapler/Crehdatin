#pragma once

#include <thread>
#include <functional>
#include <tuple>
#include <sstream>
#include <vector>
#include <fcgio.h>
#include <exception>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

#include <sys/socket.h>
#include <csignal>

#include <iostream>

#include <cgicc/Cgicc.h>
#include <cgicc/CgiDefs.h>
#include <cgicc/HTTPHeader.h>
#include <cgicc/HTTPStatusHeader.h>
#include <cgicc/CgiEnvironment.h>
#include <cgicc/CgiUtils.h>

#include "MarkupString.h"

class MarkupString;

class MarkupOutStream{
public:
	MarkupOutStream(std::ostream& outputStream);
	
	MarkupOutStream& operator<<(const char* str);
	MarkupOutStream& operator<<(const std::string str);
	MarkupOutStream& operator<<(const MarkupString& str);
	MarkupOutStream& operator<<(MarkupString&& str);
	std::ostream& getUnsafeOutputStream();
	
private:
	std::ostream& out;
};

struct FcgiData{
	FcgiData(std::ostream& outputStream, const cgicc::CgiEnvironment* _env);
	FCGX_Request* request;
	cgicc::Cgicc* cgi;
	const cgicc::CgiEnvironment* env;
	MarkupOutStream out;
};

class FcgiInput: public cgicc::CgiInput{
public:
	FCGX_Request* request;
	virtual std::size_t read(char* data, std::size_t length);
	virtual std::string getenv(const char *varName);
};

struct ThreadData{
	std::unique_ptr<std::thread> thread;
	std::unique_ptr<std::mutex> mutex;
	void* data;
};

class WebsiteFramework{
public:
	WebsiteFramework() = delete;
	
	static void setDomain(std::string name);
	static std::string getDomain();
	
	static void setThreadStartHandle(std::function<void*(std::size_t)> handle);
	static void setThreadEndHandle(std::function<void(void*, std::size_t)> handle);
	
	static void setRequestStartHandle(std::function<bool(FcgiData*, void*)> handle);
	static void setRequestEndHandle(std::function<void(FcgiData*, void*)> handle);
	
	static void setError404Handle(std::function<void(FcgiData*, void*)> handle);
	static void setExceptionHandle(std::function<void(void*, const std::exception&)> handle);
	
	static void addGetHandleMap(std::string resourcePath, std::function<void(FcgiData*, std::vector<std::string>, void*)> handle);
	static void addPostHandleMap(std::string resourcePath, std::function<void(FcgiData*, std::vector<std::string>, void*)> handle);
	static void addDirectoryHandleMap(std::string resourcePath, std::function<bool(FcgiData*, std::vector<std::string>, void*)> handle);
	
	static void run(const char* socket, std::size_t numberOfThreads, std::function<void()> backgroundHandle = nullptr);
	
	static std::vector<ThreadData> threadPool;
	
	static std::atomic<bool> stopRunning;
	static std::mutex runningMutex;
	static std::condition_variable runningCondition;
	
private:
	static void threadFunction(std::size_t threadNum, void* data);
	static bool runResourcePathHandle(FcgiData* fcgi, void* data, std::string resourcePath, std::vector<std::pair<std::vector<std::string>, std::function<void(FcgiData*, std::vector<std::string>, void*)>>>* handleMap);
	static bool runDirectoryPathHandles(FcgiData* fcgi, void* data, std::string resourcePath);
	static std::vector<std::string> resourcePathToVector(std::string resourcePath);
	static void decodeCaptured(std::vector<std::string>& captured);
	
	static std::string domainName;
	
	static int listenSocket;
	
	static std::function<void*(std::size_t)> threadStartHandle;
	static std::function<void(void*, std::size_t)> threadEndHandle;
	
	static std::function<bool(FcgiData*, void*)> requestStartHandle;
	static std::function<void(FcgiData*, void*)> requestEndHandle;
	
	static std::function<void(FcgiData*, void*)> error404Handle;
	static std::function<void(void*, const std::exception&)> exceptionHandle;
	
	static std::vector<std::pair<std::vector<std::string>, std::function<void(FcgiData*, std::vector<std::string>, void*)>>> getHandleMap;
	static std::vector<std::pair<std::vector<std::string>, std::function<void(FcgiData*, std::vector<std::string>, void*)>>> postHandleMap;
	static std::vector<std::pair<std::vector<std::string>, std::function<bool(FcgiData*, std::vector<std::string>, void*)>>> directoryHandleMap;
};

#include "Response.h"