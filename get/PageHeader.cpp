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
	
	UserPosition effectiveUserPosition = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
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
	"<meta name='viewport' content='width=device-width; maximum-scale=1; minimum-scale=1;' />"
	"<title>"
	"Creh-Datin"
	"</title>"
	"</head>"
	"<body>"
	"<header>";
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
	"</header>";
	//tab bar
	fcgi->out << "<div id='tabbar'>";
	if(data->subdatinId == -1){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>";
			printIfSelected(fcgi, selectedTab, PageTab::Main);
			fcgi->out << "Crehdatin</div></a>";
			
			if(selectedTab == PageTab::User){
				fcgi->out << "<div class='selectedTab'>User</div>";
			}
			
			if(hasAdministrationControlPermissions(effectiveUserPosition)){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/controlPanel'>";
				printIfSelected(fcgi, selectedTab, PageTab::SiteControl);
				fcgi->out << "Site Control</div></a>";
			}
	}
			
	if(data->subdatinId != -1){
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "'>";
		printIfSelected(fcgi, selectedTab, PageTab::ThreadList);
		fcgi->out << "Listing</div></a>";
		
		if(selectedTab == PageTab::Thread){
			fcgi->out << "<div class='selectedTab'>Thread</div>";
		}
		
		if(selectedTab == PageTab::Comment){
			fcgi->out << "<div class='selectedTab'>Comment</div>";
		}
		if(selectedTab != PageTab::Thread && selectedTab != PageTab::Comment){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/about'>";
			printIfSelected(fcgi, selectedTab, PageTab::About);
			fcgi->out << "About</div></a>";
			
			if(data->subdatinId != -1 && (!postLocked || hasModerationPermissions(effectiveUserPosition))){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/newThread'>";
				printIfSelected(fcgi, selectedTab, PageTab::NewThread);
				fcgi->out << "New Thread</div></a>";
			}
		}
	}
		
	if(data->userId == -1){
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/login'>";
		printIfSelected(fcgi, selectedTab, PageTab::Login);
		fcgi->out << "Login</div></a>";
	}
	else{
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/settings'>";
		printIfSelected(fcgi, selectedTab, PageTab::Settings);
		fcgi->out << "Settings</div></a>";
	}
		
		
	if(data->subdatinId == -1){
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
	
	if(data->userId == -1){
		fcgi->out <<
		"<div class='tab'>"
		"Id: " << data->shownId << 
		"</div>";
	}
	else{
		fcgi->out <<
		"<a href='https://" << WebsiteFramework::getDomain() << "/u/" << percentEncode(data->userName) << "'>"
		"<div class='tab'>"
		""  << data->userName << ""
		"</div>"
		"</a>";
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/logout'>";
		printIfSelected(fcgi, selectedTab, PageTab::Logout);
		fcgi->out << "Logout</div></a>";
	}
	
	
	fcgi->out << "</div>";
	
	//Subdatin list
	fcgi->out << 
	"<div id='subdatinDropDown'>"//does nothing, unless you are on a phone
	"<input type='checkbox' id='subdatinDropDownCheckBox'>"
	"<label id='subdatinListBtn' for='subdatinDropDownCheckBox'>"//button for use on phones
	"Subdatins"
	"</label>"
	"<ul id='subdatinList'>";
	if(data->subdatinId == -1){
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"<li id='selectedSubdatin'>"
		"<div class='name'>Creh-Datin</div>"
		"<div class='title'>crehdatin</div>"
		"</li>"
		"</a>";
	}
	else{
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"<li>"
		"<div class='name'>Creh-Datin</div>"
		"<div class='title'>crehdatin</div>"
		"</li>"
		"</a>";
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
				"<div class='name'>" << subdatinName << "</div>"
				"<div class='title'>/" << subdatinTitle << "/</div>"
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
				"<div class='name'>" << name << "</div>"
				"<div class='title'>/" << title << "/</div>"
				"</li>"
				"</a>";
			}
			else{
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << title << "'>"
				"<li>"
				"<div class='name'>" << name << "</div>"
				"<div class='title'>/" << title << "/</div>"
				"</li>"
				"</a>";
			}
		}
	}
	
	fcgi->out << "</ul>"
	"</div>"
	"<main>";
}















