#include "SettingsPage.h"

void createSettingsPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createMustBeLoggedInErrorPage(fcgi, data);
		return;
	}
	
	createSettingsPage(fcgi, data);
}

void createSettingsPage(FcgiData* fcgi, RequestData* data, std::string cssError, std::string passwordError, std::string subdatinListError){
	createPageHeader(fcgi, data, PageTab::Settings);
	
	fcgi->out << "<h1>Settings</h1>"
	"<h3>Username: " << data->userName << "</h3>"
	"<h3>Current Theme: ";
	if(data->cssTheme == "dark"){
		fcgi->out << "Dark";
	}
	else if(data->cssTheme == "light"){
		fcgi->out << "Light";
	}
	else if(data->cssTheme == "aesthicc"){
		fcgi->out << "Aesthicc";
	}
	else if(data->cssTheme == "anime"){
		fcgi->out << "Animu";
	}
	else if(data->cssTheme == "synthwave"){
		fcgi->out << "Synthwave";
	}
	else{
		fcgi->out << "ERROR";
	}
	
	fcgi->out << "</h3>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setCssTheme' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<select name='theme'>"
	"<option value='dark'>Dark</option>"
	"<option value='light'>Light</option>"
	"<option value='aesthicc'>Aesthicc</option>"
	"<option value='anime'>Animu</option>"
	"<option value='synthwave'>Synthwave</option>"
	"</select>"
	"<button type='submit'>"
	"Change Theme"
	"</button>"
	"</form>";
	if(cssError != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< cssError <<
		"</div></p>";
	}
	
	
	fcgi->out << "<br><ul>"
	"<title>Your Subdatins</title>";
	std::size_t numberOfSubdatins = 0;
	{
		std::size_t num = 0;
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId FROM subdatinListings WHERE userId = ? ORDER BY listNumber ASC"));
		prepStmt->setInt64(1, data->userId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		numberOfSubdatins = res->rowsCount();
		res->beforeFirst();
		if(res->next()){
			do{
				num++;
				std::string title = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
				fcgi->out << "<li>"
				"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setSubdatinListing' accept-charset='UTF-8'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='listNumber' value='0'>"
				"<input type='hidden' name='title' value='" << title << "'>"
				"<button type='submit'>Remove</button>"
				"</form>";
				if(num != 1){
					fcgi->out << "<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setSubdatinListing' accept-charset='UTF-8'>"
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
			fcgi->out << "<li><i>You have no subdatins...</i></li>";
		}
	}
	fcgi->out << "<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/setSubdatinListing' accept-charset='UTF-8'>"
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
	
	
	fcgi->out << "<h2>Change Password</h2>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/changePassword' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='password' name='oldPassword'>Old Password<br>"
	"<input type='password' name='newPassword'>New Password<br>"
	"<input type='password' name='repeatNewPassword'>Repeat New Password<br>"
	"<button type='submit'>"
	"Change Password"
	"</button>"
	"</form>";
	if(passwordError != ""){
		fcgi->out << "<p><div class='errorText'>" << passwordError << "</div></p>";
	}
	
	createPageFooter(fcgi, data);
}
