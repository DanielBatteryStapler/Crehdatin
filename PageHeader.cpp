#include "PageHeader.h"

void createPageHeader(FcgiData* fcgi, RequestData* data){
	sendStatusHeader(fcgi->out, StatusCode::Ok);
	sendHtmlContentTypeHeader(fcgi->out);
	finishHttpHeader(fcgi->out);
	
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
		"<button type='submit' name='submit_param' class='logout-button'>"
		"Logout"
		"</button>"
		"</form>";
		if(data->subdatinId != -1 && hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
			fcgi->out <<
			"<div class='toolbarEntry'>"
			"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/reports'>Reports</a>"
			"</div>";
		}
		if(data->subdatinId != -1 && hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
			fcgi->out <<
			"<div class='toolbarEntry'>"
			"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/controlPanel'>Control Panel</a>"
			"</div>";
		}
		if(hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
			fcgi->out <<
			"<div class='toolbarEntry'>"
			"<a href='https://" << WebsiteFramework::getDomain() << "/controlPanel'>Site Control</a>"
			"</div>";
		}
	}
	if(data->subdatinId != -1 && (!postLocked || hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId)))){
		fcgi->out <<
		"<div class='toolbarEntry'>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/newThread'>New Thread</a>"
		"</div>";
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
		"Welcome back, <a href='https://" << WebsiteFramework::getDomain() << "/user/" << percentEncode(data->userName) << "'>" << escapeHtml(data->userName) << "</a>!"
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
		"<div class='subHeaderText'><a href='https://" << WebsiteFramework::getDomain() << "/'>"
		"Creh-Datin"
		"</a></div>"
		"<div id='headerText'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "'>"
		<< escapeHtml(subdatinName) <<
		"</a></div>";
		if(commentLocked){
			fcgi->out <<
			"<div class='subHeaderText'>Comments Locked</div>";
		}
		if(postLocked){
			fcgi->out <<
			"<div class='subHeaderText'>Posts Locked</div>";
		}
	}
	fcgi->out <<
	"</header>"
	"<main>";
}
