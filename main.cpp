#include <iostream>
#include <thread>
#include <vector>
#include <fcgio.h>

#include <sys/socket.h>
#include <csignal>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/Cryptography.h>
#include <WebsiteFramework/Response.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <mysql_driver.h>

#include "Database.h"
#include "RequestData.h"

#include "DirectoryHandles.h"
#include "BackgroundHandle.h"

#include "get/MainPage.h"
#include "Captcha.h"
#include "get/PostImage.h"
#include "get/PostImageThumbnail.h"

#include "get/LoginPage.h"
#include "get/LogoutPage.h"
#include "get/CrehdatinControlPanelPage.h"

#include "get/SettingsPage.h"
#include "get/UserPage.h"

#include "get/SubdatinPage.h"
#include "get/SubdatinAboutPage.h"
#include "get/NewThreadPage.h"
#include "get/ThreadPage.h"
#include "get/CommentPage.h"
#include "get/ReportsPage.h"
#include "get/SubdatinControlPanelPage.h"

#include "post/HandleNewThread.h"
#include "post/HandleNewComment.h"

#include "post/HandleCreateAccount.h"
#include "post/HandleLogin.h"
#include "post/HandleLogout.h"

#include "post/HandleSetCssTheme.h"
#include "post/HandleChangePassword.h"
#include "post/HandleSetSubdatinListing.h"
#include "post/HandleSetShownId.h"

#include "post/HandleAddAdministrator.h"
#include "post/HandleRemoveAdministrator.h"
#include "post/HandleAddSubdatin.h"
#include "post/HandleRemoveSubdatin.h"
#include "post/HandleSetDefaultSubdatinListing.h"
#include "post/HandleBanIp.h"

#include "post/HandleAddCurator.h"
#include "post/HandleSetCurator.h"
#include "post/HandleSetBureaucrat.h"
#include "post/HandleRemoveSubdatinOfficial.h"
#include "post/HandleSetSubdatinPostLocked.h"
#include "post/HandleSetSubdatinCommentLocked.h"
#include "post/HandleSetAboutText.h"
#include "post/HandleSetThreadLocked.h"
#include "post/HandleSetThreadStickied.h"

#include "post/HandleReportThread.h"
#include "post/HandleDeleteThread.h"
#include "post/HandleReportComment.h"
#include "post/HandleDeleteComment.h"
#include "post/HandleHideThumbnails.h"
#include "post/HandleDismissReports.h"

#include "RequestStartHandle.h"

std::function<void(FcgiData*, std::vector<std::string>, void*)> createRedirectPageHandle(std::string url){
	return [url](FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + url);
		finishHttpHeader(fcgi->out);
	};
}

int main(int argc, char** argv){
	Magick::InitializeMagick(nullptr);
	
	if(argc > 1){
		if(argc == 2){
			if(argv[1] == std::string("--install")){
				Database::createDatabase();
			}
			else if(argv[1] == std::string("--uninstall")){
				Database::deleteDatabase();
			}
			else{
				std::cout << "Unknown argument \"" << argv[1] << "\"\n";
			}
		}
		else{
			std::cout << "Only accepts one argument\n";
		}
		return 0;
	}
	else{
		
		if(!Database::checkDatabase()){
			std::cout << "Database Check Failed, Aborting Website Launch...\n";
			return 0;
		}
		
		#ifdef DEBUG
		WebsiteFramework::setDomain("website.cloud.karagory.com");
		#else
		WebsiteFramework::setDomain("crehdatin.karagory.com");
		#endif // DEBUG
		WebsiteFramework::setThreadStartHandle([](std::size_t id)->void*{
			RequestData* data = new RequestData;
			
			std::cout << "Starting thread #" << std::to_string(id) << "\n";
			
			sql::Driver* driver = sql::mysql::get_driver_instance();
			data->con = driver->connect(Config::getSqlAddress(), Config::getSqlUserName(), Config::getSqlPassword());
			
			data->stmt = data->con->createStatement();
			
			data->con->setSchema(Config::getSqlDatabaseName());
			data->stmt->execute("SET NAMES utf8mb4");
			
			return (void*)data;
		});
		
		WebsiteFramework::setThreadEndHandle([](void* _data, std::size_t id){
			RequestData* data = (RequestData*)_data;
			
			std::cout << "Stopping thread #" << std::to_string(id) << "\n";
			
			delete data->stmt;
			data->stmt = nullptr;
			delete data->con;
			data->con = nullptr;
			delete data;
		});
		
		WebsiteFramework::setRequestStartHandle(requestStartHandle);
		
		WebsiteFramework::setError404Handle([](FcgiData* fcgi, void* _data){
			RequestData* data = (RequestData*)_data;
			createGenericErrorPage(fcgi, data, "Error 404, This page does not exist");
		});
		
		WebsiteFramework::setExceptionHandle([](void* _data, const std::exception& e){
			safePrint(std::string("std::exception.what() = '") + e.what() + "'\n");
		});
		
		WebsiteFramework::addDirectoryHandleMap("/d/*", subdatinDirectoryHandle);
		WebsiteFramework::addDirectoryHandleMap("/d/*/thread/*", threadDirectoryHandle);
		WebsiteFramework::addDirectoryHandleMap("/d/*/thread/*/comment/*", commentDirectoryHandle);
		WebsiteFramework::addDirectoryHandleMap("/u/*", userDirectoryHandle);
		
		WebsiteFramework::addGetHandleMap("/", createMainPage);
		WebsiteFramework::addGetHandleMap("/captcha/*", createCaptchaHandle);
		WebsiteFramework::addGetHandleMap("/postImages/*", createPostImageHandle);
		WebsiteFramework::addGetHandleMap("/postImageThumbnails/*", createPostImageThumbnailHandle);
		
		WebsiteFramework::addGetHandleMap("/login", createLoginPageHandle);
		WebsiteFramework::addGetHandleMap("/logout", createLogoutPageHandle);
		WebsiteFramework::addGetHandleMap("/createAccount", createRedirectPageHandle("/login"));
		
		WebsiteFramework::addGetHandleMap("/u/*", createUserPage);
		WebsiteFramework::addGetHandleMap("/settings", createSettingsPageHandle);
		WebsiteFramework::addGetHandleMap("/controlPanel", createCrehdatinControlPanelPageHandle);
		
		WebsiteFramework::addGetHandleMap("/d/*", createSubdatinPage);
		WebsiteFramework::addGetHandleMap("/d/*/about", createSubdatinAboutPage);
		WebsiteFramework::addGetHandleMap("/d/*/newThread", createNewThreadPageHandle);
		WebsiteFramework::addGetHandleMap("/d/*/thread/*", createThreadPage);
		WebsiteFramework::addGetHandleMap("/d/*/thread/*/comment/*", createCommentPage);
		WebsiteFramework::addGetHandleMap("/d/*/reports", createReportsPage);
		WebsiteFramework::addGetHandleMap("/d/*/controlPanel", createSubdatinControlPanelPageHandle);
		
		WebsiteFramework::addPostHandleMap("/createAccount", handleCreateAccount);
		WebsiteFramework::addPostHandleMap("/login", handleLogin);
		WebsiteFramework::addPostHandleMap("/logout", handleLogout);
		WebsiteFramework::addPostHandleMap("/setCssTheme", handleSetCssTheme);
		WebsiteFramework::addPostHandleMap("/changePassword", handleChangePassword);
		WebsiteFramework::addPostHandleMap("/setSubdatinListing", handleSetSubdatinListing);
		WebsiteFramework::addPostHandleMap("/setShownId", handleSetShownId);
		
		WebsiteFramework::addPostHandleMap("/d/*/newThread", handleNewThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/newComment", handleNewComment);
		
		WebsiteFramework::addPostHandleMap("/addAdministrator", handleAddAdministrator);
		WebsiteFramework::addPostHandleMap("/removeAdministrator", handleRemoveAdministrator);
		WebsiteFramework::addPostHandleMap("/addSubdatin", handleAddSubdatin);
		WebsiteFramework::addPostHandleMap("/d/*/removeSubdatin", handleRemoveSubdatin);
		WebsiteFramework::addPostHandleMap("/setDefaultSubdatinListing", handleSetDefaultSubdatinListing);
		WebsiteFramework::addPostHandleMap("/banIp", handleBanIp);
		
		WebsiteFramework::addPostHandleMap("/d/*/addCurator", handleAddCurator);
		WebsiteFramework::addPostHandleMap("/d/*/setCurator", handleSetCurator);
		WebsiteFramework::addPostHandleMap("/d/*/setBureaucrat", handleSetBureaucrat);
		WebsiteFramework::addPostHandleMap("/d/*/removeSubdatinOfficial", handleRemoveSubdatinOfficial);
		WebsiteFramework::addPostHandleMap("/d/*/setPostLocked", handleSetSubdatinPostLocked);
		WebsiteFramework::addPostHandleMap("/d/*/setCommentLocked", handleSetSubdatinCommentLocked);
		WebsiteFramework::addPostHandleMap("/d/*/setAboutText", handleSetAboutText);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/setThreadLocked", handleSetThreadLocked);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/setThreadStickied", handleSetThreadStickied);
		
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/reportThread", handleReportThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/deleteThread", handleDeleteThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/hideThumbnails", handleHideThumbnails);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/comment/*/reportComment", handleReportComment);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/comment/*/deleteComment", handleDeleteComment);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/comment/*/hideThumbnails", handleHideThumbnails);
		
		WebsiteFramework::addPostHandleMap("/d/*/dismissReports", handleDismissReports);
		
		WebsiteFramework::run(":8222", std::thread::hardware_concurrency() * 8, backgroundHandle);
		
		std::cout << "Shutting down...\n";
	}
	
    return 0;
}

















