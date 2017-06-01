#include "SubdatinControlPanelPage.h"

void createSubdatinControlPanelPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << "<div class='errorText'>Cannot view a control panel for a subdatin that doesn't exist</div>";
		createPageFooter(fcgi, data);
	}
	else{
		if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			createPageHeader(fcgi, data, subdatinId);
			fcgi->out << "<div class='errorText'>You do not have the correct permissions to view this page</div>";
			createPageFooter(fcgi, data);
			return;
		}
		createSubdatinControlPanelPage(fcgi, data, subdatinId, parameters[0]);
	}
}

void createSubdatinControlPanelPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string subdatinTitle){
	createPageHeader(fcgi, data, subdatinId);
	
	std::string title;
	std::string name;
	bool postsLocked;
	bool commentsLocked;
	getSubdatinData(data->con, subdatinId, title, name, postsLocked, commentsLocked);
	
	fcgi->out << "<h1>Control Panel</h1>"
	"<div class='commentEven'><h2>Subdatin Officials</h2>"
	"<div class='commentOdd'><div class='commentText'>";
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT userId, userPosition FROM userPositions WHERE subdatinId = ?"));
	prepStmt->setInt64(1, subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	if(res->next()){
		do{
			std::string userName = getUserName(data->con, res->getInt64("userId"));
			fcgi->out << 
			"<form method='post' action='https://" << Config::getDomain() << "/d/" << subdatinTitle << "/removeSubdatinOfficial' accept-charset='UTF-8'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='userName' value='" << escapeHtml(userName) << "'>"
			"<div class='postInfoElement'><button type='submit'>Remove</button></div>"
			"</form>"
			"<div class='postInfoElement'><a href=https://'" << Config::getDomain() << "/user/" << percentEncode(userName) << ">";
			if(res->getString("userPosition") == "bureaucrat"){
				fcgi->out << 
				"<form method='post' action='https://" << Config::getDomain() << "/d/" << subdatinTitle << "/setModerator' accept-charset='UTF-8'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='userName' value='" << escapeHtml(userName) << "'>"
				"<div class='postInfoElement'><button type='submit'>Set Moderator</button></div>"
				"</form>"
				"<div class='bureaucratTag'>" << userName << "[B]</div>";
			}
			else if(res->getString("userPosition") == "moderator"){
				fcgi->out << 
				"<form method='post' action='https://" << Config::getDomain() << "/d/" << subdatinTitle << "/setBureaucrat' accept-charset='UTF-8'>"
				"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
				"<input type='hidden' name='userName' value='" << escapeHtml(userName) << "'>"
				"<div class='postInfoElement'><button type='submit'>Set Bureaucrat</button></div>"
				"</form>"
				"<div class='moderatorTag'>" << userName << "[M]</div>";
			}
			else{
				fcgi->out << "<div class='errorText'>an unknown error occurred!</div>";
			}
			fcgi->out << "</div></a><br>";
		}while(res->next());
	}
	else{
		fcgi->out << "<div class='errorText'><i>There are no subdatin officials...</i></div>";
	}
	
	fcgi->out << "</div></div>"
	"<div class='commentText'>"
	"<form method='post' action='https://" << Config::getDomain() << "/d/" << subdatinTitle << "/addModerator' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<div class='postInfoElement'><input type='text' name='userName'></div>"
	"<button type='submit'>Add New</button>"
	"</form>"
	"</div>"
	"</div>"
	"<div class='commentEven'>"
	"<h2>Settings</h2>"
	"<div class='commentText'>"
	"<form method='post' action='https://" << Config::getDomain() << "/d/" << subdatinTitle << "/setPostLocked' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
	if(postsLocked){
		fcgi->out << "<button type='submit' name='locked' value='false'>Unlock Posts</button>";
	}
	else{
		fcgi->out << "<button type='submit' name='locked' value='true'>Lock Posts</button>";
	}
	fcgi->out << "</form>"
	"<form method='post' action='https://" << Config::getDomain() << "/d/" << subdatinTitle << "/setCommentLocked' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
	if(commentsLocked){
		fcgi->out << "<button type='submit' name='locked' value='false'>Unlock Comments</button>";
	}
	else{
		fcgi->out << "<button type='submit' name='locked' value='true'>Lock Comments</button>";
	}
	fcgi->out << "</form>"
	"</div>"
	"</div>";
	
	createPageFooter(fcgi, data);
}
