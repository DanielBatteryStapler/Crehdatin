#include "PageHeader.h"

void printIfSelected(FcgiData* fcgi, PageTab selected, PageTab correct){
	if(selected == correct){
		fcgi->out << "<div class='selectedTab'>";
	}
	else{
		fcgi->out << "<div class='tab'>";
	}
}

void createPageHeader(FcgiData* fcgi, RequestData* data, PageTab selectedTab){
	sendStatusHeader(fcgi->out, StatusCode::Ok);
	sendContentTypeHeader(fcgi->out, ContentType::Html);
	finishHttpHeader(fcgi->out);
	
	std::string effectiveUserPosition = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
	std::string subdatinTitle;
	std::string subdatinName;
	bool postLocked;
	bool commentLocked;
	if(data->subdatinId != -1){
		getSubdatinData(data->con, data->subdatinId, subdatinTitle, subdatinName, postLocked, commentLocked);
	}
	
	fcgi->out << 
	"<!DOCTYPE html>"
	"<html>"
	"<head>"
	"<link rel='stylesheet' type='text/css' href='https://" << WebsiteFramework::getDomain() << "/static/" << data->cssTheme << ".css'>"
	"<meta charset='UTF-8'>"
	"<meta name='viewport' content='width=device-width,initial-scale=1' />"
	"<title>"
	"Creh-Datin"
	"</title>"
	"</head>"
	"<body>"
	"<header>"
	//---Toolbar---
	"<div id='toolbar'>";
	if(data->userId == -1){
		fcgi->out << 
		"<div class='toolbarEntry'>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/login'>Login/Create Account</a>"
		"</div>";
	}
	else{
		fcgi->out <<
		"<div class='toolbarEntry'>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/settings'>Settings</a>"
		"</div>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/logout' class='logout'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' name='submit_param' class='logout-button'>"
		"Logout"
		"</button>"
		"</form>";
	}
	if(data->userId == -1){
		fcgi->out <<
		"<div class='toolbarEntry'>"
		"Welcome, Anon! Id: " << data->shownId << 
		"</div>";
	}
	else{
		fcgi->out <<
		"<div class='toolbarEntry'>"
		"Welcome back, <a href='https://" << WebsiteFramework::getDomain() << "/u/" << percentEncode(data->userName) << "'>" << data->userName << "</a>!"
		"</div>";
	}
	fcgi->out << "</div>";
	//---Header---
	if(data->subdatinId == -1){
		fcgi->out <<
		"<div id='headerText'><a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"Creh-Datin"
		"</a></div>";
	}
	else{
		fcgi->out <<
		"<div id='headerText'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "'>"
		<< subdatinName <<
		"</a></div>";
		if(commentLocked){
			fcgi->out <<
			"<div class='subHeaderText'>Comments Locked</div><br>";
		}
		if(postLocked){
			fcgi->out <<
			"<div class='subHeaderText'>Posts Locked</div><br>";
		}
	}
	fcgi->out <<
	"</header>"
	"<article>";
	//tab bar
	
	fcgi->out << "<div id='tabbar'>";
	if(data->subdatinId == -1){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>";
			printIfSelected(fcgi, selectedTab, PageTab::Main);
			fcgi->out << "Crehdatin</div></a>";
			
			if(selectedTab == PageTab::User){
				fcgi->out << "<div class='selectedTab'>User</div>";
			}
			
			if(data->userId == -1){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/login'>";
				printIfSelected(fcgi, selectedTab, PageTab::Login);
				fcgi->out << "Login/Create Account</div></a>";
			}
			else{
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/settings'>";
				printIfSelected(fcgi, selectedTab, PageTab::Settings);
				fcgi->out << "Settings</div></a>";
			}
			
			if(hasAdministrationControlPermissions(effectiveUserPosition)){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/controlPanel'>";
				printIfSelected(fcgi, selectedTab, PageTab::SiteControl);
				fcgi->out << "Site Control</div></a>";
			}
	}
	else{
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "'>";
		printIfSelected(fcgi, selectedTab, PageTab::ThreadList);
		fcgi->out << "Thread Listing</div></a>";
		
		if(selectedTab == PageTab::Thread){
			fcgi->out << "<div class='selectedTab'>Thread</div>";
		}
		
		if(selectedTab == PageTab::Comment){
			fcgi->out << "<div class='selectedTab'>Comment</div>";
		}
		
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/about'>";
		printIfSelected(fcgi, selectedTab, PageTab::About);
		fcgi->out << "About</div></a>";
		
		if(data->subdatinId != -1 && (!postLocked || hasModerationPermissions(effectiveUserPosition))){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/newThread'>";
			printIfSelected(fcgi, selectedTab, PageTab::NewThread);
			fcgi->out << "New Thread</div></a>";
		}
		
		if(hasModerationPermissions(effectiveUserPosition)){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/reports'>";
			printIfSelected(fcgi, selectedTab, PageTab::Reports);
			fcgi->out << "Reports</div></a>";
		}
		
		if(hasSubdatinControlPermissions(effectiveUserPosition)){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/controlPanel'>";
			printIfSelected(fcgi, selectedTab, PageTab::ControlPanel);
			fcgi->out << "Control Panel</div></a>";
		}
	}
	if(selectedTab == PageTab::Error){
		fcgi->out << "<div class='selectedTab'>Error</div>";
	}
	fcgi->out << "</div>";
	
	//Subdatin list
	fcgi->out << "<ul id='subdatinList'>";
	if(data->subdatinId == -1){
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"<li id='selectedSubdatin'>"
		"<title>Creh-Datin</title>"
		"<small></small>"
		"</li></a>";
	}
	else{
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"<li>"
		"<title>Creh-Datin</title>"
		"<small></small>"
		"</li></a>";
	}
	
	
	{
		std::unique_ptr<sql::PreparedStatement> prepStmt;
		if(data->userId == -1){
			prepStmt.reset(data->con->prepareStatement("SELECT subdatinId FROM subdatinListings WHERE userId IS NULL ORDER BY listNumber ASC"));
		}
		else{
			prepStmt.reset(data->con->prepareStatement("SELECT subdatinId FROM subdatinListings WHERE userId = ? ORDER BY listNumber ASC"));
			prepStmt->setInt64(1, data->userId);
		}
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		
		if(data->subdatinId != -1){
			bool inList = false;
			
			res->beforeFirst();
			while(res->next()){
				if(data->subdatinId == res->getInt64("subdatinId")){
					inList = true;
				}
			}
			
			if(!inList){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "'>"
				"<li id='selectedSubdatin'>"
				"<title>" << subdatinName << "</title>"
				"<small>/" << subdatinTitle << "/</small>"
				"</li>"
				"</a>";
			}
		}
		
		res->beforeFirst();
		while(res->next()){
			std::string title;
			std::string name;
			getSubdatinData(data->con, res->getInt64("subdatinId"), title, name);
		
			if(res->getInt64("subdatinId") == data->subdatinId){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << title << "'>"
				"<li id='selectedSubdatin'>"
				"<title>" << name << "</title>"
				"<small>/" << title << "/</small>"
				"</li>"
				"</a>";
			}
			else{
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << title << "'>"
				"<li>"
				"<title>" << name << "</title>"
				"<small>/" << title << "/</small>"
				"</li>"
				"</a>";
			}
		}
	}
	
	fcgi->out << "</ul>"
	"<main>";
}















