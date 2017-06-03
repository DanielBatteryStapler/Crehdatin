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
	sendHtmlContentTypeHeader(fcgi->out);
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
	"<html>"
	"<head>"
	"<link rel='stylesheet' type='text/css' href='https://" << WebsiteFramework::getDomain() << "/static/" << data->cssTheme << ".css'>"
	"<meta charset='UTF-8'>"
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
		"<input type='hidden' name='seeOther' value='https://" << WebsiteFramework::getDomain() << fcgi->env->getQueryString() << "'>"
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
		"Welcome back, <a href='https://" << WebsiteFramework::getDomain() << "/u/" << percentEncode(data->userName) << "'>" << escapeHtml(data->userName) << "</a>!"
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
		<< escapeHtml(subdatinName) <<
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
		"<large>Creh-Datin</large>"
		"<small></small>"
		"</li></a>";
	}
	else{
		fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"<li>"
		"<large>Creh-Datin</large>"
		"<small></small>"
		"</li></a>";
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT name, title, id FROM subdatins"));
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		if(res->getInt64("id") == data->subdatinId){
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << res->getString("title") << "'>"
			"<li id='selectedSubdatin'>"
			"<large>" << res->getString("name") << "</large>"
			"<small>/" << res->getString("title") << "/</small>"
			"</li>"
			"</a>";
		}
		else{
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << res->getString("title") << "'>"
			"<li>"
			"<large>" << res->getString("name") << "</large>"
			"<small>/" << res->getString("title") << "/</small>"
			"</li>"
			"</a>";
		}
	}
	
	fcgi->out << "</ul>"
	"<main>";
}















