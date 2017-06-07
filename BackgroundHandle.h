#include <WebsiteFramework/WebsiteFramework.h>

#include <chrono>
#include "RequestData.h"

extern std::vector<RequestData*> resquestDataReferencePool;

void backgroundHandle(std::condition_variable* backgroundCondition, std::mutex* backgroundMutex, std::atomic<bool>* backgroundFlag);