#include "BackgroundHandle.h"

void backgroundHandle(){
	std::cout << "Starting Background Handle\n";
	while(true){
		{
			std::unique_lock<std::mutex> lock(WebsiteFramework::runningMutex);
			if(WebsiteFramework::runningCondition.wait_for(lock, std::chrono::hours(1), [](){return WebsiteFramework::stopRunning.load();})){
				break;
			}
		}
		for(std::size_t i = 0; i < WebsiteFramework::threadPool.size(); i++){
			std::unique_lock<std::mutex> lock(*WebsiteFramework::threadPool[i].mutex);
			delete ((RequestData*)WebsiteFramework::threadPool[i].data)->stmt->executeQuery("SELECT UNIX_TIMESTAMP(CURRENT_TIMESTAMP)");//just make some dummy request to keep the connection alive
		}
	}
	std::cout << "Stopping Background Handle\n";
}
