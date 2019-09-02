#include "CommentPage.h"

void createCommentPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, anonId, userId, locked FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, data->threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	createPageHeader(fcgi, data, PageTab::Comment);
	
	bool canReply = renderThread(fcgi->out, data, data->threadId, UserPosition::None, ThreadFlags::isPreview);
	
	int64_t parentId = getParentComment(data->con, data->commentId);
	
	if(parentId != -1){
		fcgi->out << "<a name='" << std::to_string(data->commentId) << "'></a><div class='comment even'><div class='commentText'>"
		"<div class='underline'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << parameters[1] << "/comment/" << std::to_string(parentId) << "'>See Parent Comment...</a></div>"
		"</div>";
	}
	else{
		fcgi->out << "You are only viewing one comment, <div class='underline'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << parameters[1] << "'>View the entire thread</a></div>";
	}
	
	UserPosition position = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
	renderComment(fcgi->out, data, data->commentId, parentId != -1, canReply, position, CommentFlags::includeReplies | CommentFlags::showPoster | CommentFlags::showReplyId);
			
	createCommentLine(fcgi->out, data, canReply, position, parameters[0], 0, data->commentId);
	
	fcgi->out << "</div>";
	
	if(parentId != -1){
		fcgi->out << "</div>";
	}
	
	createPageFooter(fcgi, data);
}
