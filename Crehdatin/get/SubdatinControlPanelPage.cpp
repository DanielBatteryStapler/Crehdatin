#include "SubdatinControlPanelPage.h"

void createSubdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	createSubdatinControlPanelPage(fcgi, data, parameters[0]);
}

void createSubdatinControlPanelPage(FcgiData* fcgi, RequestData* data, std::string subdatinTitle){
	createPageHeader(fcgi, data, PageTab::ControlPanel);
	
	std::string title;
	std::string name;
	bool postsLocked;
	bool commentsLocked;
	getSubdatinData(data->con, data->subdatinId, title, name, postsLocked, commentsLocked);
	
	fcgi->out << "<h1>Control Panel</h1>"
	"<ul>"
	"<div class='title'>Subdatin Officials</div>";
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT userId, userPosition FROM userPositions WHERE subdatinId = ?"));
	prepStmt->setInt64(1, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	if(res->next()){
		do{
			std::string userName = getUserName(data->con, res->getInt64("userId"));
			fcgi->out << 
			
			"<li><div class='spacer'><a href='https://" << WebsiteFramework::getDomain() << "/u/" << percentEncode(userName) << "'>";
			if(res->getString("userPosition") == "bureaucrat"){
				fcgi->out << 
				"<div class='bureaucratTag'>" << userName << "[B]</div></a></div>"
				"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/setCurator' accept-charset='UTF-8'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='userName' value='" << userName << "'>"
				"<button type='submit'>Set Curator</button>"
				"</form>";
			}
			else if(res->getString("userPosition") == "curator"){
				fcgi->out << 
				"<div class='curatorTag'>" << userName << "[C]</div></a></div>"
				"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/setBureaucrat' accept-charset='UTF-8'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='userName' value='" << userName << "'>"
				"<button type='submit'>Set Bureaucrat</button>"
				"</form>";
			}
			else{
				fcgi->out << "<div class='errorText'>an unknown error occurred!</div></a></div>";
			}
			fcgi->out << 
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/removeSubdatinOfficial' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='userName' value='" << userName << "'>"
			"<button type='submit'>Remove</button>"
			"</form>"
			"</li>";
		}while(res->next());
	}
	else{
		fcgi->out << "<li><div class='errorText'><i>There are no subdatin officials...</i></div></li>";
	}
	
	fcgi->out << 
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/addCurator' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='userName'>"
	"<button type='submit'>Add New</button>"
	"</form>"
	"</li>"
	"</ul>"
	
	"<h2>Settings</h2>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/setPostLocked' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
	if(postsLocked){
		fcgi->out << "<button type='submit' name='locked' value='false'>Unlock Posts</button>";
	}
	else{
		fcgi->out << "<button type='submit' name='locked' value='true'>Lock Posts</button>";
	}
	fcgi->out << "</form>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/setCommentLocked' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
	if(commentsLocked){
		fcgi->out << "<button type='submit' name='locked' value='false'>Unlock Comments</button>";
	}
	else{
		fcgi->out << "<button type='submit' name='locked' value='true'>Lock Comments</button>";
	}
	fcgi->out << "</form>"
	"<h2>Information</h2>"
	"<p>This is text that is displayed under the 'About' tab to users.</p>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(title) << "/setAboutText' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<textarea name='text'>";
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("SELECT description FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, data->subdatinId);
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->first();
	if(!res->isNull("description")){
		fcgi->out << res->getString("description");
	}
	
	fcgi->out << "</textarea><br>"
	"<button type='submit'>"
	"Set About Text"
	"</button>"
	"</form>";
	
	createPageFooter(fcgi, data);
}
