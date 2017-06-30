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
#include "mysql_driver.h" 

#include "Database.h"
#include "RequestData.h"

#include "DirectoryHandles.h"
#include "BackgroundHandle.h"

#include "Captcha.h"

#include "MainPage.h"
#include "LoginPage.h"
#include "CrehdatinControlPanelPage.h"

#include "SettingsPage.h"
#include "UserPage.h"

#include "SubdatinPage.h"
#include "SubdatinAboutPage.h"
#include "NewThreadPage.h"
#include "ThreadPage.h"
#include "CommentPage.h"
#include "ReportsPage.h"
#include "SubdatinControlPanelPage.h"

#include "HandleNewThread.h"
#include "HandleNewComment.h"

#include "HandleCreateAccount.h"
#include "HandleLogin.h"
#include "HandleLogout.h"

#include "HandleSetCssTheme.h"
#include "HandleChangePassword.h"
#include "HandleSetSubdatinListing.h"

#include "HandleAddAdministrator.h"
#include "HandleRemoveAdministrator.h"
#include "HandleAddSubdatin.h"
#include "HandleRemoveSubdatin.h"
#include "HandleSetDefaultSubdatinListing.h"

#include "HandleAddModerator.h"
#include "HandleSetModerator.h"
#include "HandleSetBureaucrat.h"
#include "HandleRemoveSubdatinOfficial.h"
#include "HandleSetSubdatinPostLocked.h"
#include "HandleSetSubdatinCommentLocked.h"
#include "HandleSetAboutText.h"
#include "HandleSetThreadLocked.h"
#include "HandleSetThreadStickied.h"

#include "HandleReportThread.h"
#include "HandleDeleteThread.h"
#include "HandleReportComment.h"
#include "HandleDeleteComment.h"
#include "HandleDismissReports.h"

#include "RequestStartHandle.h"

std::function<void(FcgiData*, std::vector<std::string>, void*)> createRedirectPageHandle(std::string url){
	return [url](FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/" + url);
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
		
		sql::Driver* driver = sql::mysql::get_driver_instance();
		
		WebsiteFramework::setDomain("website.cloud.karagory.com");
		//WebsiteFramework::setDomain("crehdatin.karagory.com");
		
		WebsiteFramework::setThreadStartHandle([driver]()->void*{
			RequestData* data = new RequestData;
			
			data->con = driver->connect(Config::getSqlAddress(), Config::getSqlUserName(), Config::getSqlPassword());
			
			data->stmt = data->con->createStatement();
			
			data->con->setSchema(Config::getSqlDatabaseName());								
			data->stmt->execute("SET NAMES utf8mb4");
			
			resquestDataReferencePool.push_back(data);
			
			return (void*)data;
		});
		
		WebsiteFramework::setThreadEndHandle([](void* _data){
			RequestData* data = (RequestData*)_data;
			
			delete data->stmt;
			data->stmt = nullptr;
			delete data->con;
			data->con = nullptr;
			delete data;
		});
		
		WebsiteFramework::setRequestStartHandle(requestStartHandle);
		
		WebsiteFramework::setError404Handle([](FcgiData* fcgi, void* _data){
			RequestData* data = (RequestData*)_data;
			createPageHeader(fcgi, data, PageTab::Error);
			fcgi->out << "<div class='errorText'>This Page Does Not Exist</div>"_m;
			createPageFooter(fcgi, data);
		});
		
		WebsiteFramework::setExceptionHandle([](void* _data, std::exception& e){
			std::cout << "std::exception.what()" << e.what() << "\n";
		});
		
		WebsiteFramework::addDirectoryHandleMap("/d/*", subdatinDirectoryHandle);
		WebsiteFramework::addDirectoryHandleMap("/d/*/thread/*", threadDirectoryHandle);
		WebsiteFramework::addDirectoryHandleMap("/d/*/thread/*/comment/*", commentDirectoryHandle);
		WebsiteFramework::addDirectoryHandleMap("/u/*", userDirectoryHandle);
		
		WebsiteFramework::addGetHandleMap("/", createMainPage);
		WebsiteFramework::addGetHandleMap("/captcha/*", createCaptchaHandle);
		WebsiteFramework::addGetHandleMap("/createAccount", createRedirectPageHandle("login"));
		WebsiteFramework::addGetHandleMap("/login", createLoginPageHandle);
		WebsiteFramework::addGetHandleMap("/settings", createSettingsPageHandle);
		WebsiteFramework::addGetHandleMap("/controlPanel", createCrehdatinControlPanelPageHandle);
		WebsiteFramework::addGetHandleMap("/d/*", createSubdatinPage);
		WebsiteFramework::addGetHandleMap("/d/*/about", createSubdatinAboutPage);
		WebsiteFramework::addGetHandleMap("/d/*/newThread", createNewThreadPageHandle);
		WebsiteFramework::addGetHandleMap("/d/*/thread/*", createThreadPage);
		WebsiteFramework::addGetHandleMap("/d/*/thread/*/comment/*", createCommentPage);
		WebsiteFramework::addGetHandleMap("/d/*/reports", createReportsPage);
		WebsiteFramework::addGetHandleMap("/d/*/controlPanel", createSubdatinControlPanelPageHandle);
		WebsiteFramework::addGetHandleMap("/u/*", createUserPage);
		
		WebsiteFramework::addPostHandleMap("/createAccount", handleCreateAccount);
		WebsiteFramework::addPostHandleMap("/login", handleLogin);
		WebsiteFramework::addPostHandleMap("/logout", handleLogout);
		WebsiteFramework::addPostHandleMap("/setCssTheme", handleSetCssTheme);
		WebsiteFramework::addPostHandleMap("/changePassword", handleChangePassword);
		WebsiteFramework::addPostHandleMap("/setSubdatinListing", handleSetSubdatinListing);
		
		WebsiteFramework::addPostHandleMap("/d/*/newThread", handleNewThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/newComment", handleNewComment);
		
		WebsiteFramework::addPostHandleMap("/addAdministrator", handleAddAdministrator);
		WebsiteFramework::addPostHandleMap("/removeAdministrator", handleRemoveAdministrator);
		WebsiteFramework::addPostHandleMap("/addSubdatin", handleAddSubdatin);
		WebsiteFramework::addPostHandleMap("/d/*/removeSubdatin", handleRemoveSubdatin);
		WebsiteFramework::addPostHandleMap("/setDefaultSubdatinListing", handleSetDefaultSubdatinListing);
		
		WebsiteFramework::addPostHandleMap("/d/*/addModerator", handleAddModerator);
		WebsiteFramework::addPostHandleMap("/d/*/setModerator", handleSetModerator);
		WebsiteFramework::addPostHandleMap("/d/*/setBureaucrat", handleSetBureaucrat);
		WebsiteFramework::addPostHandleMap("/d/*/removeSubdatinOfficial", handleRemoveSubdatinOfficial);
		WebsiteFramework::addPostHandleMap("/d/*/setPostLocked", handleSetSubdatinPostLocked);
		WebsiteFramework::addPostHandleMap("/d/*/setCommentLocked", handleSetSubdatinCommentLocked);
		WebsiteFramework::addPostHandleMap("/d/*/setAboutText", handleSetAboutText);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/setThreadLocked", handleSetThreadLocked);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/setThreadStickied", handleSetThreadStickied);
		
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/reportThread", handleReportThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/deleteThread", handleDeleteThread);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/comment/*/reportComment", handleReportComment);
		WebsiteFramework::addPostHandleMap("/d/*/thread/*/comment/*/deleteComment", handleDeleteComment);
		WebsiteFramework::addPostHandleMap("/d/*/dismissReports", handleDismissReports);
		
		WebsiteFramework::run(":8222", std::thread::hardware_concurrency() * 16, backgroundHandle);
		resquestDataReferencePool.clear();
		
		std::cout << "Shutting down...\n";
	}
	
    return 0;
}

















