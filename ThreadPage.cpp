#include "ThreadPage.h"

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string userPosition = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
	createPageHeader(fcgi, data, PageTab::Thread);
	
	bool canModerate = hasModerationPermissions(userPosition);
	bool canControl = hasSubdatinControlPermissions(userPosition);
	
	bool canReply = renderThread(fcgi->out, data, data->subdatinId, parameters[0], data->threadId, false, canModerate, canControl);
	
	createCommentLine(fcgi->out, data, canModerate, canReply, parameters[0]);
	
	createPageFooter(fcgi, data);
}

void createCommentLine(std::ostream& fcgiOut, RequestData* data, bool canModerate, bool canReply, std::string& subdatinTitle, int64_t layer, int64_t parentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt;
		
	if(parentId == -1){
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"SELECT id FROM comments WHERE parentId IS NULL AND threadId = ? ORDER BY createdTime DESC"));
		prepStmt->setInt64(1, data->threadId);
	}
	else{
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
			"SELECT id FROM comments WHERE parentId = ? AND threadId = ? ORDER BY createdTime DESC"));
		prepStmt->setInt64(1, parentId);
		prepStmt->setInt64(2, data->threadId);
	}
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		if(layer >= 10){
			fcgiOut << (layer%2==0?"<div class='comment even'>":"<div class='comment odd'>")
			<< "<div class='commentText'>"
			"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(parentId) << "'>"
			"View Replies"
			"</a>"
			"</div>";
		}
		else{
			do{
				renderComment(fcgiOut, data, data->subdatinId, subdatinTitle, res->getInt64("id"), layer%2==0, false, true, true, canReply, canModerate);
						
				createCommentLine(fcgiOut, data, canModerate, canReply, subdatinTitle, layer + 1, res->getInt64("id"));
				
				fcgiOut << "</div>";
			}while(res->next());
		}
	}
}


bool renderThread(std::ostream& fcgiOut, RequestData* data, std::int64_t subdatinId, std::string& subdatinTitle, std::size_t threadId, bool isPreview, bool canModerate, bool canControl, bool showSubdatin){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, body, anonId, userId, locked, stickied FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	bool postLocked;
	bool commentLocked;
	getSubdatinLockedData(data->con, subdatinId, postLocked, commentLocked);
	
	bool canReply = !res->getBoolean("locked") && !commentLocked || canModerate;
	
	std::string anonId;
	if(!res->isNull("anonId")){
		anonId = res->getString("anonId");
	}
	int64_t userId;
	if(!res->isNull("userId")){
		userId = res->getInt64("userId");
	}
	
	if(isPreview){
		fcgiOut << 
		"<a style='display:block' href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/thread/" << std::to_string(threadId) << "'>"
		"<div class='thread'>"
			"<div class='threadTitle'>" << escapeHtml(res->getString("title")) << "</div>"
			"<div class='extraPostInfo'>";
				if(showSubdatin){
					fcgiOut << "<div class='postInfoElement'>/" << escapeHtml(subdatinTitle) << "/</div>";
				}
				fcgiOut << "<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, subdatinId, false) << "</div>"
				"<div class='postInfoElement'>comments: " << std::to_string(getThreadCommentCount(data->con, threadId)) << "</div>"
				"<div class='postInfoElement'>" << getFormattedThreadPostTime(data->con, threadId) << "</div>"
				"<div class='postInfoElement'>" << getFormattedThreadBumpTime(data->con, threadId) << "</div>";
				if(res->getBoolean("stickied")){
					fcgiOut << "<div class='postInfoElement'>Stickied</div>";
				}
				if(res->getBoolean("locked")){
					fcgiOut << "<div class='postInfoElement'>Locked</div>";
				}
			fcgiOut << "</div>"
			"</div>"
		"</a>";
	}
	else{
		fcgiOut << "<div class='thread'>"
			"<div class='threadTitle'>" << escapeHtml(res->getString("title")) << "</div>"
			"<div class='extraPostInfo'>";
				if(showSubdatin){
					fcgiOut << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << escapeHtml(subdatinTitle) << "'><div class='postInfoElement'>/" << escapeHtml(subdatinTitle) << "/</div></a>";
				}
				fcgiOut << 
				"<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, subdatinId) << "</div>"
				"<div class='postInfoElement'>comments: " << std::to_string(getThreadCommentCount(data->con, threadId)) << "</div>";
				if(canReply){
					createReplyMenu(fcgiOut, data, subdatinTitle);
				}
				createReportMenu(fcgiOut, data, subdatinTitle);
				if(canModerate){
					createThreadModerationMenu(fcgiOut, data, subdatinTitle, threadId, canControl, res->getBoolean("locked"), res->getBoolean("stickied"));
				}
				fcgiOut << "<div class='postInfoElement'>" << getFormattedThreadPostTime(data->con, threadId) << "</div>"
				"<div class='postInfoElement'>" << getFormattedThreadBumpTime(data->con, threadId) << "</div>";
				if(res->getBoolean("locked")){
					fcgiOut << "<div class='postInfoElement'>Locked</div>";
				}
				if(res->getBoolean("stickied")){
					fcgiOut << "<div class='postInfoElement'>Stickied</div>";
				}
			fcgiOut << "</div>"
			"<div class='threadText'>" << formatUserPostBody(escapeHtml(res->getString("body")), hasRainbowTextPermissions(getEffectiveUserPosition(data->con, userId, data->subdatinId))) << "</div>"
		"</div>";
	}
	
	return canReply;
}

void renderComment(std::ostream& fcgiOut, RequestData* data, std::int64_t subdatinId, std::string& subdatinTitle, std::size_t commentId, bool isEven, bool isPreview, bool showPoster, bool showPermaLink, bool canReply, bool canModerate, bool showSubdatin){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT threadId, body, anonId, userId FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, commentId);
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	int64_t threadId = res->getInt64("threadId");
	std::string body = res->getString("body");
	std::string anonId;
	if(!res->isNull("anonId")){
		anonId = res->getString("anonId");
	}
	int64_t userId = -1;
	if(!res->isNull("userId")){
		userId = res->getInt64("userId");
	}
	
	fcgiOut << "<a name='" << std::to_string(commentId) << "'></a>"
	<< (isEven?"<div class='comment even'>":"<div class='comment odd'>") << 
	"<div class='extraPostInfo'>";
	if(showSubdatin){
		fcgiOut << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << escapeHtml(subdatinTitle) << "'><div class='postInfoElement'>/" << escapeHtml(subdatinTitle) << "/</div></a>";
	}
	if(showPoster){
		fcgiOut << "<div class='postInfoElement'>" << getFormattedPosterString(data->con, anonId, userId, subdatinId) << "</div>";
	}
	if(!isPreview){
		if(canReply){
			createReplyMenu(fcgiOut, data, subdatinTitle, commentId);
		}
		createReportMenu(fcgiOut, data, subdatinTitle, commentId);
		if(canModerate){
			createCommentModerationMenu(fcgiOut, data, subdatinTitle, threadId, commentId);
		}
	}
	if(showPermaLink){
		fcgiOut << "<div class='postInfoElement'>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "'>Permalink</a>"
		"</div>";
	}
	
	fcgiOut << "<div class='postInfoElement'>" << getFormattedCommentPostTime(data->con, commentId) << "</div>" 
	"</div>"
	"<div class='commentText'>" << formatUserPostBody(escapeHtml(body), hasRainbowTextPermissions(getEffectiveUserPosition(data->con, userId, subdatinId))) << "</div>";
}

void createReportMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t commentId){
	fcgiOut <<
		"<div class='postInfoElement'>"
		"<div class='dropDown'>"
		"<div class='dropBtn'>"
		"Report"
		"</div>"
		"<ul>";
	
	if(commentId == -1){
		fcgiOut <<
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/reportThread' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<div class='subjectInput'><input type='text' name='subject'></div>"
			"<li><button type='submit' name='reason' value='Illegal' class='link-button'>Illegal</button></li>"
			"<li><button type='submit' name='reason' value='Spam' class='link-button'>Spam</button></li>"
			"<li><button type='submit' name='reason' value='Copyrighted' class='link-button'>Copyrighted</button></li>"
			"<li><button type='submit' name='reason' value='Obscene' class='link-button'>Obscene</button></li>"
			"<li><button type='submit' name='reason' value='Inciting Violence' class='link-button'>Inciting Violence</button></li>"
			"<li><button type='submit' class='link-button'>Other, specify:</button><input type='text' name='reason' class='inline'></li>"
			"</form>";
	}
	else{
		fcgiOut <<
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
			"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
			"<div class='subjectInput'><input type='text' name='subject'></div>"
			"<li><button type='submit' name='reason' value='Illegal' class='link-button'>Illegal</button></li>"
			"<li><button type='submit' name='reason' value='Spam' class='link-button'>Spam</button></li>"
			"<li><button type='submit' name='reason' value='Copyrighted' class='link-button'>Copyrighted</button></li>"
			"<li><button type='submit' name='reason' value='Obscene' class='link-button'>Obscene</button></li>"
			"<li><button type='submit' name='reason' value='Inciting Violence' class='link-button'>Inciting Violence</button></li>"
			"<li><button type='submit' class='link-button'>Other, specify:</button><input type='text' name='reason' class='inline'></li>"
			"</form>";
	}
	
	fcgiOut << 
		"</ul>"
		"</div>"
		"</div>";
	
}

void createReplyMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t commentId){
	fcgiOut << "<div class='postInfoElement'>"
	"<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Reply"
	"</div>"
	"<ul>"
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(data->threadId) << "/newComment' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='hidden' name='parentId' value='" << std::to_string(commentId) << "'>"
	"<div class='subjectInput'><input type='text' name='subject'></div>"
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

void createThreadModerationMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, bool canControl, bool locked, bool stickied){
	fcgiOut << "<div class='postInfoElement'>"
	"<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Moderate"
	"</div>"
	"<ul>"
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/deleteThread' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' class='link-button'>"
		"Delete"
		"</button>"
		"</form>"
		"</li>";
		fcgiOut <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/setThreadLocked' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
		if(locked){
			fcgiOut << "<button type='submit' class='link-button' name='locked' value='false'>"
			"Unlock Thread"
			"</button>";
		}
		else{
			fcgiOut << "<button type='submit' class='link-button' name='locked' value='true'>"
			"Lock Thread"
			"</button>";
		}
		fcgiOut << "</form>"
		"</li>";
		fcgiOut <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/setThreadStickied' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>";
		if(canControl){
			if(stickied){
				fcgiOut << "<button type='submit' class='link-button' name='stickied' value='false'>"
				"Make Thread Non-sticky"
				"</button>";
			}
			else{
				fcgiOut << "<button type='submit' class='link-button' name='stickied' value='true'>"
				"Make Thread Sticky"
				"</button>";
			}
		}
		fcgiOut << "</form>"
		"</li>"
	"</ul>"
	"</form>"
	"</div>"
	"</div>";
}

void createCommentModerationMenu(std::ostream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, int64_t commentId){
	fcgiOut << "<div class='postInfoElement'>"
	"<div class='dropDown'>"
	"<div class='dropBtn'>"
	"Moderate"
	"</div>"
	"<ul>"
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "/deleteComment' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' class='link-button'>"
		"Delete"
		"</button>"
		"</form>"
		"</li>"
	"</ul>"
	"</form>"
	"</div>"
	"</div>";
}