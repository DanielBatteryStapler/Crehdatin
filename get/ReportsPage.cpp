#include "ReportsPage.h"

void createReportsPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	createPageHeader(fcgi, data, PageTab::Reports);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id, reason, commentId, threadId, ip, userId FROM reports WHERE subdatinId = ? ORDER BY threadId, commentId"));
	prepStmt->setInt64(1, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	int64_t lastCommentId = -1;
	int64_t lastThreadId = -1;
	
	if(res->next()){
		do{
			int64_t reportId = res->getInt64("id");
			std::string reason = res->getString("reason");
			int64_t commentId = -1;
			if(!res->isNull("commentId")){
				commentId = res->getInt64("commentId");
			}
			int64_t threadId = res->getInt64("threadId");
			std::string ip = res->getString("ip");
			int64_t userId = -1;
			if(!res->isNull("userId")){
				userId = res->getInt64("userId");
			}
			
			if(lastCommentId != commentId || lastThreadId != threadId){
				if(lastThreadId != -1){
					fcgi->out << "</div>";
				}
				
				if(commentId == -1){
					createReportedThread(fcgi, data, threadId);
				}
				else{
					createReportedComment(fcgi, data, threadId, commentId);
				}
			}
			
			createReport(fcgi, data, reason, ip, userId);
			
			lastCommentId = commentId;
			lastThreadId = threadId;
		}while(res->next());
	}
	else{
		fcgi->out << "There are no pending reports.";
	}
	
	createPageFooter(fcgi, data);
}

void createReportedThread(FcgiData* fcgi, RequestData* data, int64_t threadId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, body, userId, posterIp, subdatinId FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	if(res->next()){
		int64_t userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt64("userId");
		}
		std::string subdatinTitle = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
		
		std::string userName;
		
		if(userId != -1){
			userName = getUserName(data->con, userId);
		}
		
		fcgi->out << "<div class='thread'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "'>"
			<< res->getString("title") << "</a><br><div class='extraPostInfo'>";
		
		fcgi->out << "<div class='postInfoElement'><div class='dropDown'><div class='dropBtn'>Actions</div><ul><li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/deleteThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' class='link-button'>"
			"Delete"
			"</button>"
			"</form></li>";
			
		fcgi->out << "<li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/dismissReports' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='threadId' value='" << std::to_string(threadId) << "'>"
			"<input type='hidden' name='commentId' value='-1'>"
			"<button type='submit' class='link-button'>"
			"Dismiss"
			"</button>"
			"</form></li>";
			
		fcgi->out << "</ul></div></div><div class='postInfoElement'>";
		if(userId != -1){
			fcgi->out << "<div class='postInfoElement'>" << getFormattedPosterString(data->con, "", userId, res->getInt64("subdatinId")) << "</div>";
		}
		
		fcgi->out << "ip: " << res->getString("posterIp") << 
			"</div></div>"
			"<div class='threadText'>" << formatUserPostBody(data, res->getString("body"), hasRainbowTextPermissions(getEffectiveUserPosition(data->con, userId, res->getInt64("subdatinId")))) << "</div>";
	}
	else{
		fcgi->out << "<div class='thread'><div class='threadText'><div class='errorText'>"
		"An error occurred while showing this thread</div></div>";
	}
}

void createReportedComment(FcgiData* fcgi, RequestData* data, int64_t threadId, int64_t commentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT body, userId, posterIp, subdatinId FROM comments WHERE id = ? AND threadId = ?"));
	prepStmt->setInt64(1, commentId);
	prepStmt->setInt64(2, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	if(res->next()){
		int64_t userId = -1;
		if(!res->isNull("userId")){
			userId = res->getInt64("userId");
		}
		std::string subdatinTitle = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
		
		std::string userName;
		
		if(userId != -1){
			userName = getUserName(data->con, userId);
		}
		
		fcgi->out << "<div class='comment even'><div class='extraPostInfo'><div class='postInfoElement'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "'>comment</a></div>"
			"<div class='postInfoElement'><div class='dropDown'>"
			"<input type='checkbox' id='actionsMenu" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
			"<label class='dropBtn' for='actionsMenu" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
			"Actions"
			"</label>"
			"<ul><li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "/deleteComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' class='link-button'>"
			"Delete"
			"</button>"
			"</form></li>";
			
		fcgi->out << "<li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/dismissReports' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<input type='hidden' name='threadId' value='" << std::to_string(threadId) << "'>"
			"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
			"<button type='submit' class='link-button'>"
			"Dismiss"
			"</button>"
			"</form></li>";
			
		fcgi->out << "</ul></div></div><div class='postInfoElement'>";
		if(userId != -1){
			fcgi->out << "user: " << userName << ", ";
		}
		
		fcgi->out << "ip: " << res->getString("posterIp") << 
			"</div></div>"
			"<div class='commentText'>" << formatUserPostBody(data, res->getString("body"), hasRainbowTextPermissions(getEffectiveUserPosition(data->con, userId, res->getInt64("subdatinId")))) <<
			"</div>";
	}
	else{
		fcgi->out << "<div class='thread'><div class='threadText'><div class='errorText'>"
		"An error occurred while showing this thread</div></div>";
	}
}

void createReport(FcgiData* fcgi, RequestData* data, std::string& reason, std::string& ip, int64_t userId){
	fcgi->out << "<div class='comment odd'><div class='commentText'>'" << reason << "' ";
	if(userId != -1){
		std::string userName;
		
		userName = getUserName(data->con, userId);
		
		fcgi->out << "user: " << userName << ", ";
	}
	fcgi->out << "ip: " << ip <<
	 "</div></div>";
}













