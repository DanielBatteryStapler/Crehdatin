#include "UserPage.h"

void createUserPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::User);
	
	std::string globalUserPosition = getEffectiveUserPosition(data->con, data->userPageId, -1);
	std::string userName = getUserName(data->con, data->userPageId);
	
	if(globalUserPosition == "senate"){
		fcgi->out << "<h2><div class='senateTag'>" << escapeHtml(userName) << "[S]</div></h2>";
	}
	else if(globalUserPosition == "administrator"){
		fcgi->out << "<h2><div class='administratorTag'>" << escapeHtml(userName) << "[A]</div></h2>";
	}
	else if(globalUserPosition == ""){
		fcgi->out << "<h2>" << escapeHtml(userName) << "</h2>";
	}
	else{
		fcgi->out << "<h2>An Error Occurred</h2>";
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId, userPosition FROM userPositions WHERE userId = ?"));
	prepStmt->setInt64(1, data->userPageId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		fcgi->out << "<ul id='positionList'><div id='positionListTitle'>Subdatin Positions:</div></h3>";
		do{
			std::string title = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
			fcgi->out << "<li><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(title) << "'>/" << escapeHtml(title) << "/</a> : ";
			if(res->getString("userPosition") == "bureaucrat"){
				fcgi->out << "Bureaucrat";
			}
			else if(res->getString("userPosition") == "moderator"){
				fcgi->out << "Moderator";
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
				int64_t threadId = res->getInt64("threadId");
				std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("SELECT title FROM threads WHERE id = ?"));
				prepStmtB->setInt64(1, threadId);
				std::unique_ptr<sql::ResultSet> resB(prepStmtB->executeQuery());
				resB->first();
				fcgi->out << "<a style='display:block' href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "'>"
				"<div class='thread'><div class='threadTitle'>" << escapeHtml(resB->getString("title")) << "</div>"
				"<div class='extraPostInfo'><div class='postInfoElement'>Posted To /" << escapeHtml(subdatinTitle) << "/</div></div></div></a>";
				
			}
			else if(!res->isNull("commentId")){
				int64_t commentId = res->getInt64("commentId");
				std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("SELECT body, threadId FROM comments WHERE id = ?"));
				prepStmtB->setInt64(1, commentId);
				std::unique_ptr<sql::ResultSet> resB(prepStmtB->executeQuery());
				resB->first();
				fcgi->out << "<div class='comment even'><div class='extraPostInfo'>"
				"<div class='postInfoElement'>"
				"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(resB->getInt64("threadId")) << "/comment/" << std::to_string(commentId) << "'>Permalink</a></div>"
				"<div class='postInfoElement'>Posted to /" << escapeHtml(subdatinTitle) << "/</div>"
				"</div><div class='commentText'>"
				<< formatUserPostBody(escapeHtml(resB->getString("body")), getEffectiveUserPosition(data->con, data->userPageId, subdatinId)) << 
				"</div></div>";
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
