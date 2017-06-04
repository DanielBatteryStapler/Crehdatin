#include "ThreadPage.h"

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string userPosition = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, body, anonId, userId, locked, stickied FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, data->threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	createPageHeader(fcgi, data, PageTab::Thread);
	
	std::string title = res->getString("title");
	std::string body = formatUserPostBody(escapeHtml(res->getString("body")), userPosition);
	std::string anonId;
	if(!res->isNull("anonId")){
		anonId = res->getString("anonId");
	}
	int userId = -1;
	if(!res->isNull("userId")){
		userId = res->getInt("userId");
	}
	
	bool postLocked;
	bool commentLocked;
	getSubdatinLockedData(data->con, data->subdatinId, postLocked, commentLocked);
	
	bool canReply = (!res->getBoolean("locked") && !commentLocked) || hasModerationPermissions(userPosition);
	
	fcgi->out << "<div class='thread'><div class='threadTitle'>"
		<< escapeHtml(title) << "</div><div class='extraPostInfo'>"
		"<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, data->subdatinId) << "</div>"
		"<div class='postInfoElement'>comments: " << std::to_string(getThreadCommentCount(data->con, data->threadId)) << "</div>";
	if(canReply){
		createReplyMenu(fcgi, data, parameters[0]);
	}
	createReportMenu(fcgi, data, parameters[0]);
	if(hasModerationPermissions(userPosition)){
		createModerationMenu(fcgi, data, parameters[0], userPosition, -1, res->getBoolean("locked"), res->getBoolean("stickied"));
	}
	if(res->getBoolean("locked")){
		fcgi->out << "<div class='postInfoElement'>Locked</div>";
	}
	if(res->getBoolean("stickied")){
		fcgi->out << "<div class='postInfoElement'>Stickied</div>";
	}
	fcgi->out << 
		"</div>"
		"<div class='threadText'>" << body
		<< "</div></div>";
		
	createCommentLine(fcgi, data, userPosition, canReply, parameters[0]);
	
	createPageFooter(fcgi, data);
}

void createCommentLine(FcgiData* fcgi, RequestData* data, std::string& userPosition, bool canReply, std::string& subdatinTitle, int64_t layer, int64_t parentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt;
		
	if(parentId == -1){
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"SELECT id, body, anonId, userId FROM comments WHERE parentId IS NULL AND threadId = ? ORDER BY createdTime DESC"));
		prepStmt->setInt64(1, data->threadId);
	}
	else{
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"SELECT id, body, anonId, userId FROM comments WHERE parentId = ? AND threadId = ? ORDER BY createdTime DESC"));
		prepStmt->setInt64(1, parentId);
		prepStmt->setInt64(2, data->threadId);
	}
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		if(layer >= 10){
			fcgi->out << (layer%2==0?"<div class='comment even'>":"<div class='comment odd'>")
			<< "<div class='commentText'>"
			"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(parentId) << "'>"
			"View Replies"
			"</a>"
			"</div>";
		}
		else{
			do{
				int64_t commentId = res->getInt64("id");
				std::string body = res->getString("body");
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
				
				body = formatUserPostBody(escapeHtml(body), getEffectiveUserPosition(data->con, userId, data->subdatinId));
				
				fcgi->out << "<a name='" << std::to_string(commentId) << "'></a>"
				<< (layer%2==0?"<div class='comment even'>":"<div class='comment odd'>") << 
				"<div class='extraPostInfo'>"
				"<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, data->subdatinId) << "</div>";
				if(canReply){
					createReplyMenu(fcgi, data, subdatinTitle, commentId);
				}
				createReportMenu(fcgi, data, subdatinTitle, commentId);
				if(hasModerationPermissions(userPosition)){
					createModerationMenu(fcgi, data, subdatinTitle, userPosition, commentId);
				}
				
				fcgi->out << 
				"<div class='postInfoElement'>"
				"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "'>Permalink</a>"
				"</div>"
				"</div>"
				"<div class='commentText'>" << body << "</div>";
						
				createCommentLine(fcgi, data, userPosition, canReply, subdatinTitle, layer + 1, commentId);
				
				fcgi->out << "</div>";
			}while(res->next());
		}
	}
}

void createReportMenu(FcgiData* fcgi, RequestData* data, std::string& subdatinTitle, int64_t commentId){
	
	fcgi->out <<
		"<div class='postInfoElement'>"
		"<div class='dropDown'>"
		"<div class='dropBtn'>"
		"Report"
		"</div>"
		"<ul>";
	
	if(commentId == -1){
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Illegal' class='link-button'>"
			"Illegal"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Spam' class='link-button'>"
			"Spam"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Copyrighted' class='link-button'>"
			"Copyrighted"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Obscene' class='link-button'>"
			"Obscene"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Inciting Violence' class='link-button'>"
			"Inciting Violence"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' class='link-button'>"
			"Other, specify:"
			"</button>"
			"<input type='text' name='reason' class='inline'>"
			"</form>"
			"</li>";
	}
	else{
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Illegal' class='link-button'>"
			"Illegal"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Spam' class='link-button'>"
			"Spam"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Copyrighted' class='link-button'>"
			"Copyrighted"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Obscene' class='link-button'>"
			"Obscene"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' name='reason' value='Inciting Violence' class='link-button'>"
			"Inciting Violence"
			"</button>"
			"</form>"
			"</li>";
		fcgi->out <<
			"<li>"
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<button type='submit' class='link-button'>"
			"Other, specify:"
			"</button>"
			"<input type='text' name='reason' class='inline'>"
			"</form>"
			"</li>";
	}
	
	fcgi->out << 
		"</ul>"
		"</div>"
		"</div>";
	
}

void createReplyMenu(FcgiData* fcgi, RequestData* data, std::string& subdatinTitle, int64_t commentId){
	fcgi->out << "<div class='postInfoElement'>"
	"<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Reply"
	"</div>"
	"<ul>"
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/newComment' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='hidden' name='parentId' value='" << std::to_string(commentId) << "'>"
	"<textarea cols='25' name='body'></textarea><br>"
	"<button type='submit' name='submit_param'>"
	"Create Comment"
	"</button>"
	"</li>"
	"</ul>"
	"</form>"
	"</div>"
	"</div>";
}

void createModerationMenu(FcgiData* fcgi, RequestData* data, std::string& subdatinTitle, std::string& userPosition, int64_t commentId, bool locked, bool stickied){
	fcgi->out << "<div class='postInfoElement'>"
	"<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Moderate"
	"</div>"
	"<ul>";
	if(commentId == -1){
		fcgi->out <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/deleteThread' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' class='link-button'>"
		"Delete"
		"</button>"
		"</form>"
		"</li>";
		fcgi->out <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/setThreadLocked' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
		if(locked){
			fcgi->out << "<button type='submit' class='link-button' name='locked' value='false'>"
			"Unlock Thread"
			"</button>";
		}
		else{
			fcgi->out << "<button type='submit' class='link-button' name='locked' value='true'>"
			"Lock Thread"
			"</button>";
		}
		fcgi->out << "</form>"
		"</li>";
		fcgi->out <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/setThreadStickied' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
		if(hasSubdatinControlPermissions(userPosition)){
			if(stickied){
				fcgi->out << "<button type='submit' class='link-button' name='stickied' value='false'>"
				"Make Thread Non-sticky"
				"</button>";
			}
			else{
				fcgi->out << "<button type='submit' class='link-button' name='stickied' value='true'>"
				"Make Thread Sticky"
				"</button>";
			}
		}
		fcgi->out << "</form>"
		"</li>";
	}
	else{
		fcgi->out <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/deleteComment' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' class='link-button'>"
		"Delete"
		"</button>"
		"</form>"
		"</li>";
	}
	fcgi->out << "</ul>"
	"</form>"
	"</div>"
	"</div>";
}


