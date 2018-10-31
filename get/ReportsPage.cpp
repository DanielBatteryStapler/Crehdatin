#include "ReportsPage.h"

void createReportsPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	UserPosition position = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
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
					fcgi->out << "<div class='comment even'>";
					renderThread(fcgi->out, data, threadId, position, ThreadFlags::showDismissReport);
				}
				else{
					renderComment(fcgi->out, data, commentId, true, false, position, CommentFlags::includeReplies | CommentFlags::showDismissReport | CommentFlags::showPoster | CommentFlags::showReplyId);
				}
			}
			
			createReport(fcgi, data, reason, ip, userId);
			
			lastCommentId = commentId;
			lastThreadId = threadId;
		}while(res->next());
		if(lastThreadId != -1){
			fcgi->out << "</div>";
		}
	}
	else{
		fcgi->out << "There are no pending reports.";
	}
	
	createPageFooter(fcgi, data);
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














