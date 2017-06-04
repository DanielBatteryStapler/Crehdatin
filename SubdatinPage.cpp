#include "SubdatinPage.h"

void createSubdatinPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::ThreadList);
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id, title, anonId, userId, locked, stickied FROM threads WHERE subdatinId = ? ORDER BY stickied DESC, lastBumpTime DESC"));
	prepStmt->setInt64(1, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	if(!res->next()){
		fcgi->out << "<div class='errorText'><i>There don't appear to be any posts here...</i></div>";
	}
	else{
		do{
			int64_t threadId = res->getInt64("id");
			std::string title = res->getString("title");
			std::string anonId;
			if(!res->isNull("anonId")){
				anonId = res->getString("anonId");
			}
			std::string userName;
			int64_t userId = -1;
			if(!res->isNull("userId")){
				userId = res->getInt64("userId");
				userName = getUserName(data->con, userId);
			}
			
			fcgi->out << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << std::to_string(threadId) << "'>"
			"<div class='thread'>"
			"<div class='threadTitle'>" << escapeHtml(title) << "</div>"
			"<div class='extraPostInfo'>"
			"<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, data->subdatinId, false) << "</div>"
			"<div class='postInfoElement'>comments: " << std::to_string(getThreadCommentCount(data->con, threadId)) << "</div>";
			
			if(res->getBoolean("stickied")){
				fcgi->out << "<div class='postInfoElement'>Stickied</div>";
			}
			if(res->getBoolean("locked")){
				fcgi->out << "<div class='postInfoElement'>Locked</div>";
			}
			fcgi->out << "</div>"
			"</div>"
			"</a>";
			
		}while(res->next());
	}
	createPageFooter(fcgi, data);
}
