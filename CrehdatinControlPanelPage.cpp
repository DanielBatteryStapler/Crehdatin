#include "CrehdatinControlPanelPage.h"

void createCrehdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'>You do not have the correct permissions to view this page</div>";
		createPageFooter(fcgi, data);
		return;
	}
	createCrehdatinControlPanelPage(fcgi, data);
}

void createCrehdatinControlPanelPage(FcgiData* fcgi, RequestData* data){
	createPageHeader(fcgi, data);
	
	fcgi->out << "<h1>Crehdatin Control Panel</h1>"
	"<div class='commentEven'><h2>Administrators</h2>"
	"<div class='commentOdd'><div class='commentText'>";
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT userName FROM users WHERE userPosition = 'administrator'"));
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		do{
			fcgi->out << 
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/removeAdministrator' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='userName' value='" << escapeHtml(res->getString("userName")) << "'>"
			"<div class='postInfoElement'><button type='submit'>Remove</button></div>"
			"</form>"
			"<a href=https://'" << WebsiteFramework::getDomain() << "/user/" << percentEncode(res->getString("userName")) << ">"
			<< escapeHtml(res->getString("userName")) <<
			"</a><br>";
		}while(res->next());
	}
	else{
		fcgi->out << "<div class='errorText'><i>There are no administrators...</i></div>";
	}
	
	fcgi->out << "</div></div>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/addAdministrator' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<div class='postInfoElement'><input type='text' name='userName'></div>"
	"<button type='submit'>Add New</button>"
	"</form>"
	"</div>";
	
	createPageFooter(fcgi, data);
}
