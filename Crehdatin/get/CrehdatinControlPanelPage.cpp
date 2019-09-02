#include "CrehdatinControlPanelPage.h"

void createCrehdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	createCrehdatinControlPanelPage(fcgi, data);
}

void createCrehdatinControlPanelPage(FcgiData* fcgi, RequestData* data, std::string subdatinListError){
	createPageHeader(fcgi, data, PageTab::SiteControl);
	
	fcgi->out << "<h1>Crehdatin Control Panel</h1>"
	"<ul>"
	"<div class='title'>Subdatins</div>";
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, name FROM subdatins"));
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		do{
			fcgi->out << 
			"<li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << res->getString("title") << "/removeSubdatin' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit'>Remove</button>"
			"</form>"
			"<a href=https://" << WebsiteFramework::getDomain() << "/u/" << res->getString("title") << "'>"
			<< res->getString("name") <<
			"</a></li>";
		}while(res->next());
	}
	else{
		fcgi->out << "<li><div class='errorText'><i>There are no subdatins...</i></div></li>";
	}
	
	fcgi->out << 
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/addSubdatin' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"title: "
	"<input type='text' name='title'>"
	"name: "
	"<input type='text' name='name'>"
	"<button type='submit'>Add New</button>"
	"</form>"
	"</li>"
	"</ul><br>"
	
	"<ul>"
	"<div class='title'>Administrators</div>";
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("SELECT userName FROM users WHERE userPosition = 'administrator'"));
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		do{
			fcgi->out << 
			"<li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/removeAdministrator' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='userName' value='" << res->getString("userName") << "'>"
			"<button type='submit'>Remove</button>"
			"</form>"
			"<a href=https://'" << WebsiteFramework::getDomain() << "/u/" << percentEncode(res->getString("userName")) << ">"
			<< res->getString("userName") <<
			"</a></li>";
		}while(res->next());
	}
	else{
		fcgi->out << "<li><div class='errorText'><i>There are no administrators...</i></div></li>";
	}
	
	fcgi->out <<
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/addAdministrator' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='userName'>"
	"<button type='submit'>Add New</button>"
	"</form>"
	"</li>"
	"</ul><br>";
	
	fcgi->out << "<ul>"
	"<div class='title'>Default Subdatin List</div>";
	std::size_t numberOfSubdatins = 0;
	{
		std::size_t num = 0;
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId FROM subdatinListings WHERE userId IS NULL ORDER BY listNumber ASC"));
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		numberOfSubdatins = res->rowsCount();
		res->beforeFirst();
		if(res->next()){
			do{
				num++;
				std::string title = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
				fcgi->out << "<li>"
				"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setDefaultSubdatinListing' accept-charset='UTF-8'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='listNumber' value='0'>"
				"<input type='hidden' name='title' value='" << title << "'>"
				"<button type='submit'>Remove</button>"
				"</form>";
				if(num != 1){
					fcgi->out << "<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setDefaultSubdatinListing' accept-charset='UTF-8'>"
					"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
					"<input type='hidden' name='listNumber' value='" << std::to_string(num - 1) << "'>"
					"<input type='hidden' name='title' value='" << title << "'>"
					"<button type='submit'>Up</button>"
					"</form>";
				}
				fcgi->out << "/" << title << "/</li>";
			}while(res->next());
		}
		else{
			fcgi->out << "<li><i>There are no default subdatins...</i></li>";
		}
	}
	fcgi->out << "<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setDefaultSubdatinListing' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='hidden' name='listNumber' value='" << std::to_string(numberOfSubdatins + 1) << "'>"
	"<input type='text' name='title'>"
	"<button type='submit'>Add New</button>"
	"</form>"
	"</li>"
	"</ul>";
	if(subdatinListError != ""){
		fcgi->out << "<p><div class='errorText'>" << subdatinListError << "</div></p>";
	}
	
	fcgi->out << "<br><ul><div class='title'>Ip Banning</div><li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/banIp' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='ip'>"
	"<button type='submit'>Ban Ip</button>"
	"</form></li></ul>";
	
	createPageFooter(fcgi, data);
}
