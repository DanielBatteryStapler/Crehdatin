#include "BackgroundHandle.h"

std::vector<RequestData*> resquestDataReferencePool;

void backgroundHandle(std::condition_variable* backgroundCondition, std::mutex* backgroundMutex, std::atomic<bool>* backgroundFlag){
	while(true){
		{
			std::unique_lock<std::mutex> lock(*backgroundMutex);
			if(backgroundCondition->wait_for(lock, std::chrono::hours(1), [backgroundFlag](){return backgroundFlag->load();})){
				break;
			}
		}
		for(std::size_t i = 0; i < std::thread::hardware_concurrency() * 16; i++){
			std::unique_lock<std::mutex> lock(*WebsiteFramework::mutexPool[i]);
			delete resquestDataReferencePool[i]->stmt->executeQuery("SELECT UNIX_TIMESTAMP(CURRENT_TIMESTAMP)");//just make some dummy request to keep the connection alive
		}
	}
}
