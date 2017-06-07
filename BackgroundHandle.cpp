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
		std::cout << "Running keep alive requests to all mysql connections...\n";
		for(std::size_t i = 0; i < std::thread::hardware_concurrency() * 16; i++){
			std::cout << "\tStarting mysql request for thread " << std::to_string(i) << " \n";
			WebsiteFramework::mutexPool[i]->lock();
			resquestDataReferencePool[i]->stmt->execute("SELECT UNIX_TIMESTAMP(CURRENT_TIMESTAMP)");//just make some dummy request to keep the connection alive
			WebsiteFramework::mutexPool[i]->unlock();
			std::cout << "\tDone with mysql request for thread " << std::to_string(i) << " \n";
		}
	}
}
