#include "PageHeader.h"

void printIfSelected(FcgiData* fcgi, PageTab selected, PageTab correct){
	if(selected == correct){
		fcgi->out << "<li id='selectedTab'>";
	}
	else{
		fcgi->out << "<li>";
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
	"<meta name='viewport' content='width=device-width, maximum-scale=1, minimum-scale=1, initial-scale=1' />"
	"<title>"
	<< getPageTitle(fcgi, data, selectedTab) <<
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
	fcgi->out << "<div id='tabbarDropDown'>"
	"<input type='checkbox' id='tabbarDropDownCheckBox'>"
	"<label id='tabbarButton' for='tabbarDropDownCheckBox'>"//button for use on phones
	"Tabs"
	"</label>"
	"<ul id='tabbar'>";
	if(data->subdatinId == -1){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>";
			printIfSelected(fcgi, selectedTab, PageTab::Main);
			fcgi->out << "Crehdatin</li></a>";
			
			if(selectedTab == PageTab::User){
				fcgi->out << "<li id='selectedTab'>User</li>";
			}
			
			if(hasAdministrationControlPermissions(effectiveUserPosition)){
				fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/controlPanel'>";
				printIfSelected(fcgi, selectedTab, PageTab::SiteControl);
				fcgi->out << "Site Control</li></a>";
			}
	}
			
	if(data->subdatinId != -1){
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "'>";
		printIfSelected(fcgi, selectedTab, PageTab::ThreadList);
		fcgi->out << "Listing</li></a>";
		
		if(selectedTab == PageTab::Thread){
			fcgi->out << "<li id='selectedTab'>Thread</li>";
		}
		
		if(selectedTab == PageTab::Comment){
			fcgi->out << "<li id='selectedTab'>Comment</li>";
		}
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/about'>";
		printIfSelected(fcgi, selectedTab, PageTab::About);
		fcgi->out << "About</li></a>";
		
		if(data->subdatinId != -1 && (!postLocked || hasModerationPermissions(effectiveUserPosition))){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/newThread'>";
			printIfSelected(fcgi, selectedTab, PageTab::NewThread);
			fcgi->out << "New Thread</li></a>";
		}
	}
		
	if(data->subdatinId != -1){
		if(hasModerationPermissions(effectiveUserPosition)){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/reports'>";
			printIfSelected(fcgi, selectedTab, PageTab::Reports);
			fcgi->out << "Reports</li></a>";
		}
		
		if(hasSubdatinControlPermissions(effectiveUserPosition)){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/controlPanel'>";
			printIfSelected(fcgi, selectedTab, PageTab::ControlPanel);
			fcgi->out << "Control Panel</li></a>";
		}
	}
	if(selectedTab == PageTab::Error){
		fcgi->out << "<li id='selectedTab'>Error</li>";
	}
	
	if(data->userId == -1){
		fcgi->out <<
		"<li>"
		"Id: " << data->shownId.substr(0, 8) << 
		"</li>";
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/login'>";
		printIfSelected(fcgi, selectedTab, PageTab::Login);
		fcgi->out << "Login</li></a>";
	}
	else{
		fcgi->out <<
		"<a href='https://" << WebsiteFramework::getDomain() << "/u/" << percentEncode(data->userName) << "'>"
		"<li>"
		<< data->userName <<
		"</li>"
		"</a>";
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/logout'>";
		printIfSelected(fcgi, selectedTab, PageTab::Logout);
		fcgi->out << "Logout</li></a>";
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/settings'>";
		printIfSelected(fcgi, selectedTab, PageTab::Settings);
		fcgi->out << "Settings</li></a>";
	}
	
	
	fcgi->out << "</div>"
	"</div>";
	
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

std::string getPageTitle(FcgiData* fcgi, RequestData* data, PageTab selectedTab){
	switch(selectedTab){
	case None:
		return "Creh-Datin";
	case Main:
		return "Creh-Datin - Main";
	case User:
		return "Creh-Datin - User Page";
	case Login:
		return "Creh-Datin - Login";
	case Logout:
		return "Creh-Datin - Logout";
	case Settings:
		return "Creh-Datin - User Settings";
	case SiteControl:
		return "Creh-Datin - Site Control";
	case Error:
		return "Creh-Datin - Error";
	case ThreadList:
		return "Creh-Datin - Thread Listings";
	case Thread:
		return "Creh-Datin - Thread";
	case Comment:
		return "Creh-Datin - Comment";
	case About:
		return "Creh-Datin - About";
	case NewThread:
		return "Creh-Datin - New Thread";
	case Reports:
		return "Creh-Datin - Reports";
	case ControlPanel:
		return "Creh-Datin - Subdatin Control Panel";
	default:
		return "Creh-Datin - Title Error";
	}
}













