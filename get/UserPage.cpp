#include "UserPage.h"

void createUserPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::User);
	
	UserPosition globalUserPosition = getEffectiveUserPosition(data->con, data->userPageId, -1);
	std::string userName = getUserName(data->con, data->userPageId);
	
	if(globalUserPosition == UserPosition::Senate){
		fcgi->out << "<h2><div class='senateTag'>" << userName << "[S]</div></h2>";
	}
	else if(globalUserPosition == UserPosition::Administrator){
		fcgi->out << "<h2><div class='administratorTag'>" << userName << "[A]</div></h2>";
	}
	else if(globalUserPosition == UserPosition::None){
		fcgi->out << "<h2>" << userName << "</h2>";
	}
	else{
		fcgi->out << "<h2>An Error Occurred</h2>";
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId, userPosition FROM userPositions WHERE userId = ?"));
	prepStmt->setInt64(1, data->userPageId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		fcgi->out << "<ul><title>Subdatin Positions:</title></h3>";
		do{
			std::string title = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
			fcgi->out << "<li><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(title) << "'>/" << title << "/</a> : ";
			if(res->getString("userPosition") == "bureaucrat"){
				fcgi->out << "Bureaucrat";
			}
			else if(res->getString("userPosition") == "curator"){
				fcgi->out << "Curator";
			}
			else{
				fcgi->out << "An Error Occurred";
			}
			fcgi->out << "</li>";
		}while(res->next());
		fcgi->out << "</ul>";
	}
	else{
	}
	
	fcgi->out << "<h3>Posts:</h3>";
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
		"SELECT id AS threadId, NULL AS commentId, subdatinId, createdTime FROM threads WHERE userId = ? UNION "
		"SELECT NULL AS threadId, id AS commentId, subdatinId, createdTime FROM comments WHERE userId = ? ORDER BY createdTime DESC"));
	prepStmt->setInt64(1, data->userPageId);
	prepStmt->setInt64(2, data->userPageId);
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		do{
			int64_t subdatinId = res->getInt64("subdatinId");
			std::string subdatinTitle = getSubdatinTitle(data->con, subdatinId);
			if(!res->isNull("threadId")){
				renderThread(fcgi->out, data, subdatinId, subdatinTitle, res->getInt64("threadId"), true, false, false, true);
			}
			else if(!res->isNull("commentId")){
				renderComment(fcgi->out, data, subdatinId, subdatinTitle, res->getInt64("commentId"), false, true, false, true, false, false, true);
				fcgi->out << "</div>";
			}
			else{
				fcgi->out << "An Error Occured While Loading This Post";
			}
		}while(res->next());
	}
	else{
		fcgi->out << "<i>This User Has Made No Posts...</i>";
	}
	
	createPageFooter(fcgi, data);
}
