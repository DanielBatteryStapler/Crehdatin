#include "WebsiteFramework.h"

MarkupOutStream::MarkupOutStream(std::ostream& outputStream):
	out(outputStream){
}

MarkupOutStream& MarkupOutStream::operator<<(const char* str){
	out << str;
	return *this;
}

MarkupOutStream& MarkupOutStream::operator<<(const std::string str){
	out << MarkupString::escapeMarkup(str);
	return *this;
}

MarkupOutStream& MarkupOutStream::operator<<(const MarkupString& str){
	out << str.buffer;
	return *this;
}

MarkupOutStream& MarkupOutStream::operator<<(MarkupString&& str){
	out << std::move(str.buffer);
	return *this;
}

std::ostream& MarkupOutStream::getUnsafeOutputStream(){
	return out;
}

FcgiData::FcgiData(std::ostream& outputStream, const cgicc::CgiEnvironment* _env):
	out(outputStream),
	env(_env){
}

std::size_t FcgiInput::read(char* data, std::size_t length){
	return FCGX_GetStr(data, length, request->in);
}

std::string FcgiInput::getenv(const char *varName){
	const char* temp = FCGX_GetParam(varName, request->envp);
	if(temp == nullptr){
		return std::string();
	}
	else{
		return std::string(temp);
	}
}

//std::vector<std::mutex*> WebsiteFramework::mutexPool;
std::vector<ThreadData> WebsiteFramework::threadPool;

std::string WebsiteFramework::domainName;

int WebsiteFramework::listenSocket;
std::atomic<bool> WebsiteFramework::stopRunning;
std::mutex WebsiteFramework::runningMutex;
std::condition_variable WebsiteFramework::runningCondition;

std::function<void*(std::size_t)> WebsiteFramework::threadStartHandle;
std::function<void(void*, std::size_t)> WebsiteFramework::threadEndHandle;

std::function<bool(FcgiData*, void*)> WebsiteFramework::requestStartHandle;
std::function<void(FcgiData*, void*)> WebsiteFramework::requestEndHandle;

std::function<void(FcgiData*, void*)> WebsiteFramework::error404Handle;
std::function<void(void*, const std::exception&)> WebsiteFramework::exceptionHandle;

//std::vector<std::thread*> WebsiteFramework::threadPool;
std::vector<std::pair<std::vector<std::string>, std::function<void(FcgiData*, std::vector<std::string>, void*)>>> WebsiteFramework::getHandleMap;
std::vector<std::pair<std::vector<std::string>, std::function<void(FcgiData*, std::vector<std::string>, void*)>>> WebsiteFramework::postHandleMap;
std::vector<std::pair<std::vector<std::string>, std::function<bool(FcgiData*, std::vector<std::string>, void*)>>> WebsiteFramework::directoryHandleMap;

void WebsiteFramework::setDomain(std::string name){
	domainName = name;
}

std::string WebsiteFramework::getDomain(){
	return domainName;
}

void WebsiteFramework::setThreadStartHandle(std::function<void*(std::size_t)> handle){
	threadStartHandle = handle;
}

void WebsiteFramework::setThreadEndHandle(std::function<void(void*, std::size_t)> handle){
	threadEndHandle = handle;
}

void WebsiteFramework::setRequestStartHandle(std::function<bool(FcgiData*, void*)> handle){
	requestStartHandle = handle;
}

void WebsiteFramework::setRequestEndHandle(std::function<void(FcgiData*, void*)> handle){
	requestEndHandle = handle;
}

void WebsiteFramework::setError404Handle(std::function<void(FcgiData*, void*)> handle){
	error404Handle = handle;
}

void WebsiteFramework::setExceptionHandle(std::function<void(void*, const std::exception&)> handle){
	exceptionHandle = handle;
}

void WebsiteFramework::addGetHandleMap(std::string resourcePath, std::function<void(FcgiData*, std::vector<std::string>, void*)> handle){
	getHandleMap.push_back(std::make_pair(resourcePathToVector(resourcePath), handle));
}

void WebsiteFramework::addPostHandleMap(std::string resourcePath, std::function<void(FcgiData*, std::vector<std::string>, void*)> handle){
	postHandleMap.push_back(std::make_pair(resourcePathToVector(resourcePath), handle));
}

void WebsiteFramework::addDirectoryHandleMap(std::string resourcePath, std::function<bool(FcgiData*, std::vector<std::string>, void*)> handle){
	directoryHandleMap.push_back(std::make_pair(resourcePathToVector(resourcePath), handle));
}

void WebsiteFramework::run(const char* socket, std::size_t numberOfThreads, std::function<void()> backgroundHandle){
	
	stopRunning = false;
	listenSocket = FCGX_OpenSocket(socket, 500);
	FCGX_Init();
	
	auto signalHandler = [](int signum){
		std::unique_lock<std::mutex> lock(runningMutex);
		std::cout << "\nSIGNAL INPUT:Start Shutdown Procedure\n";
		stopRunning = true;
		runningCondition.notify_all();
	};
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	for(std::size_t i = 0; i < numberOfThreads; i++){
		ThreadData newThread;
		
		if(threadStartHandle){
			newThread.data = threadStartHandle(i);
		}
		else{
			newThread.data = nullptr;
		}
		newThread.mutex = std::make_unique<std::mutex>();
		newThread.thread = std::make_unique<std::thread>(WebsiteFramework::threadFunction, i, newThread.data);
		
		threadPool.push_back(std::move(newThread));
	}
	
	{
		std::unique_ptr<std::thread> backgroundThread;
		
		if(backgroundHandle){
			backgroundThread = std::make_unique<std::thread>(backgroundHandle);
		}
		
		{
			std::unique_lock<std::mutex> lock(runningMutex);
			runningCondition.wait(lock, [](){return stopRunning.load();});
		}
		
		FCGX_ShutdownPending();//tell FCGX that we are shutting down
		shutdown(listenSocket, SHUT_RDWR);//shutdown the sock used by fastcgi
		
		if(backgroundHandle){
			backgroundThread->join();
		}
	}
	
	for(int i = 0; i < threadPool.size(); i++){
		//threadPool[i].mutex->lock();//no reason to lock the mutex, the background handle has already been stopped and the thread may get deadlocked if I lock it
		threadPool[i].thread->join();
		threadPool[i].mutex.reset();
		threadPool[i].thread.reset();
		if(threadEndHandle){
			threadEndHandle(threadPool[i].data, i);
			threadPool[i].data = nullptr;
		}
	}
	threadPool.clear();
}

void WebsiteFramework::threadFunction(std::size_t threadNum, void* data){
	FCGX_Request request;
	FCGX_InitRequest(&request, listenSocket, 0);
	
	while(FCGX_Accept_r(&request) == 0){
		std::unique_lock<std::mutex> lock(*threadPool[threadNum].mutex);
		#ifndef DEBUG
		try{
		#endif // DEBUG
			fcgi_streambuf fcgiOutBuffer(request.out);
			std::ostream outputStream(&fcgiOutBuffer);
			
			FcgiInput input;
			input.request = &request;
			
			cgicc::Cgicc cgi(&input);
			
			FcgiData fcgi(outputStream, &cgi.getEnvironment());
			fcgi.request = &request;
			fcgi.cgi = &cgi;
			
			bool continueRequest = true;
			if(requestStartHandle){
				continueRequest = requestStartHandle(&fcgi, data);
			}
			
			if(continueRequest){
				if(fcgi.env->getRequestMethod() == "GET"){
					if(!runResourcePathHandle(&fcgi, data, fcgi.env->getQueryString(), &getHandleMap)){
						if(error404Handle){
							error404Handle(&fcgi, data);
						}
					}
				}
				else if(fcgi.env->getRequestMethod() == "POST"){
					if(!runResourcePathHandle(&fcgi, data, fcgi.env->getQueryString(), &postHandleMap)){
						if(error404Handle){
							error404Handle(&fcgi, data);
						}
					}
				}
				else{
					sendStatusHeader(fcgi.out, StatusCode::MethodNotAllowed);
					finishHttpHeader(fcgi.out);
				}
				
				if(requestEndHandle){
					requestEndHandle(&fcgi, data);
				}
			}
		#ifndef DEBUG
		}
		catch(const std::exception& e){
			if(exceptionHandle){
				exceptionHandle(data, e);
			}
		}
		#endif // DEBUG
		FCGX_Finish_r(&request);
	}
}

bool WebsiteFramework::runResourcePathHandle(FcgiData* fcgi, void* data, std::string resourcePath, std::vector<std::pair<std::vector<std::string>, std::function<void(FcgiData*, std::vector<std::string>, void*)>>>* handleMap){
	std::vector<std::string> resource = resourcePathToVector(resourcePath);
	std::vector<std::string> captured;
	
	for(auto i = handleMap->begin(); i != handleMap->end(); i++){
		if(resource.size() != i->first.size()){
			continue;
		}
		
		bool shouldRun = true;
		
		for(auto y = i->first.begin(), w = resource.begin(); y != i->first.end(); y++, w++){
			if(*y == "*"){
				captured.push_back(*w);
			}
			else if(*w != *y){
				shouldRun = false;
				break;
			}
		}
		
		if(shouldRun){
			if(!runDirectoryPathHandles(fcgi, data, resourcePath)){
				return true;
			}
			decodeCaptured(captured);
			i->second(fcgi, captured, data);
			captured.clear();
			return true;
		}
	}
	
	return false;
}

bool WebsiteFramework::runDirectoryPathHandles(FcgiData* fcgi, void* data, std::string resourcePath){
	std::vector<std::string> resource = resourcePathToVector(resourcePath);
	std::vector<std::string> captured;
	
	for(auto i = directoryHandleMap.begin(); i != directoryHandleMap.end(); i++){
		if(resource.size() < i->first.size()){
			continue;
		}
		
		bool shouldRun = true;
		
		for(auto y = i->first.begin(), w = resource.begin(); y != i->first.end(); y++, w++){
			if(*y == "*"){
				captured.push_back(*w);
			}
			else if(*w != *y){
				shouldRun = false;
				break;
			}
		}
		
		if(shouldRun){
			decodeCaptured(captured);
			if(!i->second(fcgi, captured, data)){
				return false;
			}
			captured.clear();
		}
	}
	
	return true;
}

std::vector<std::string> WebsiteFramework::resourcePathToVector(std::string resourcePath){
	std::vector<std::string> output;
	std::stringstream pathStream(resourcePath);
	
	std::string temp;
	while(std::getline(pathStream, temp, '/')){
		if(temp.size() != 0){
			output.push_back(temp);
		}
	}
	
	return output;
}

void WebsiteFramework::decodeCaptured(std::vector<std::string>& captured){
	for(auto i = captured.begin(); i != captured.end(); i++){
		*i = percentDecode(*i);
	}
}






