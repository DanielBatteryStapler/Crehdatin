#include "CommentPage.h"

void createCommentPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string userPosition = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, anonId, userId, locked FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, data->threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	createPageHeader(fcgi, data, PageTab::Comment);
	
	std::string title = res->getString("title");
	std::string anonId;
	if(!res->isNull("anonId")){
		anonId = res->getString("anonId");
	}
	int userId = -1;
	if(!res->isNull("userId")){
		userId = res->getInt64("userId");
	}
	bool locked = res->getBoolean("locked");
	
	fcgi->out << "<div class='thread'><div class='threadTitle'>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << std::to_string(data->threadId) << "'>"
		<< escapeHtml(title) << "</a></div><div class='extraPostInfo'>"
		"<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, data->subdatinId) << "</div>"
		"<div class='postInfoElement'>comments: " << std::to_string(getThreadCommentCount(data->con, data->threadId)) << "</div>"
		"</div></div>";
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("SELECT parentId, body, anonId, userId FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, data->commentId);
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->first();
	
	int64_t parentId = -1;
	if(!res->isNull("parentId")){
		parentId = res->getInt64("parentId");
	}
	std::string body = formatUserPostBody(escapeHtml(res->getString("body")), userPosition);
	anonId.clear();
	if(!res->isNull("anonId")){
		anonId = res->getString("anonId");
	}
	userId = -1;
	if(!res->isNull("userId")){
		userId = res->getInt64("userId");
	}
	
	if(parentId != -1){
		fcgi->out << "<a name='" << std::to_string(data->commentId) << "'></a><div class='comment even'><div class='commentText'>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << parameters[1] << "/comment/" << std::to_string(parentId) << "'>See Parent Comment...</a>";
	}
	else{
		fcgi->out << "You are only viewing one comment, <a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << parameters[1] << "'>View the entire thread</a>";
	}
	
	bool postLocked;
	bool commentLocked;
	getSubdatinLockedData(data->con, data->subdatinId, postLocked, commentLocked);
	
	bool canReply = (!locked && !commentLocked) || hasModerationPermissions(userPosition);
	
	fcgi->out << "<div class='comment odd'>"
	"<div class='extraPostInfo'>"
	"<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, data->subdatinId) << "</div>";
	if(canReply){
		createReplyMenu(fcgi, data, parameters[0], data->commentId);
	}
	createReportMenu(fcgi, data, parameters[0], data->commentId);
	if(hasModerationPermissions(userPosition)){
		createModerationMenu(fcgi, data, parameters[0], userPosition, data->commentId);
	}
	
	fcgi->out << "</div>"
	"<div class='commentText'>" << body << "</div>";
			
	createCommentLine(fcgi, data, userPosition, canReply, parameters[0], 0, data->commentId);
	
	fcgi->out << "</div>";
	
	if(parentId != -1){
		fcgi->out << "</div></div>";
	}
	
	
	createPageFooter(fcgi, data);
}
