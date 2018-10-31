#include "ThreadPage.h"

namespace{
	inline bool flagContains(ThreadFlags a, ThreadFlags b){
		return static_cast<bool>(a & b);
	}
	inline bool flagContains(CommentFlags a, CommentFlags b){
		return static_cast<bool>(a & b);
	}
};

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	UserPosition userPosition = getEffectiveUserPosition(data->con, data->userId, data->subdatinId);
	
	createPageHeader(fcgi, data, PageTab::Thread);
	
	bool canReply = renderThread(fcgi->out, data, data->threadId, userPosition, ThreadFlags::none);
	
	createCommentLine(fcgi->out, data, canReply, userPosition, parameters[0]);
	
	createPageFooter(fcgi, data);
}

void createCommentLine(MarkupOutStream& fcgiOut, RequestData* data, bool canReply, UserPosition position, std::string& subdatinTitle, int64_t layer, int64_t parentId){
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
				renderComment(fcgiOut, data, res->getInt64("id"), layer%2==0, canReply, position, CommentFlags::showPoster | CommentFlags::showReplyId | CommentFlags::includeReplies);

				createCommentLine(fcgiOut, data, canReply, position, subdatinTitle, layer + 1, res->getInt64("id"));

				fcgiOut << "</div>";
			}while(res->next());
		}
	}
}


bool renderThread(MarkupOutStream& fcgiOut, RequestData* data, std::size_t threadId, UserPosition position, ThreadFlags flags){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId, title, body, anonId, userId, locked, stickied FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(!res->next()){
		fcgiOut << "Unknown error when rendering thread.\n";
		return false;
	}
	
	int64_t subdatinId = res->getInt64("subdatinId");
	std::string subdatinTitle = getSubdatinTitle(data->con, subdatinId);
	
	bool postLocked;
	bool commentLocked;
	getSubdatinLockedData(data->con, subdatinId, postLocked, commentLocked);
	
	bool canReply = (!res->getBoolean("locked") && !commentLocked) || hasModerationPermissions(position);
	
	std::string anonId;
	if(!res->isNull("anonId")){
		anonId = res->getString("anonId");
	}
	int64_t userId = -1;
	if(!res->isNull("userId")){
		userId = res->getInt64("userId");
	}
	
	if(flagContains(ThreadFlags::isPreview, flags)){
		fcgiOut << 
		"<a style='display:block' href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "/thread/" << std::to_string(threadId) << "'>"
		"<div class='thread'>";
		createPostImages(fcgiOut, data, threadId, -1, flagContains(ThreadFlags::isPreview, flags));
		fcgiOut << "<div class='title'>";
		if(res->getBoolean("stickied")){
			fcgiOut << "<div class='greenText'>";
		}
		fcgiOut << res->getString("title");
		if(res->getBoolean("stickied")){
			fcgiOut << "</div>";
		}
		fcgiOut << "</div>"
		"<div class='extraPostInfo'>";
		if(flagContains(ThreadFlags::showSubdatin, flags)){
			fcgiOut << "<li>/" << subdatinTitle << "/</li>";
		}
		fcgiOut << "<li>" << getFormattedPosterString(data->con, anonId, userId, subdatinId, false) << "</li>"
		"<li>comments: " << std::to_string(getThreadCommentCount(data->con, threadId)) << "</li>"
		"<li>" << getFormattedThreadPostTime(data->con, threadId) << "</li>"
		"<li>" << getFormattedThreadBumpTime(data->con, threadId) << "</li>";
		if(res->getBoolean("stickied")){
			fcgiOut << "<li>Stickied</li>";
		}
		if(res->getBoolean("locked")){
			fcgiOut << "<li>Locked</li>";
		}
		fcgiOut << "</div>"
		"</div>"
		"</a>";
	}
	else{
		fcgiOut << "<div class='thread'>";
		createPostImages(fcgiOut, data, threadId, -1, flagContains(ThreadFlags::isPreview, flags));
		fcgiOut << "<div class='title'>" << res->getString("title") << "</div><br>"
		"<div class='extraPostInfo'>";
		if(flagContains(ThreadFlags::showSubdatin, flags)){
			fcgiOut << "<a href='https://" << WebsiteFramework::getDomain() << "/d/" << subdatinTitle << "'><li>/" << subdatinTitle << "/</li></a>";
		}
		fcgiOut << 
		"<li>" << getFormattedPosterString(data->con, anonId, userId, subdatinId) << "</li>"
		"<li>comments: " << std::to_string(getThreadCommentCount(data->con, threadId)) << "</li>";
		if(canReply){
			createReplyMenu(fcgiOut, data, subdatinTitle, subdatinId, threadId);
		}
		createReportMenu(fcgiOut, data, subdatinTitle, threadId);
		if(hasModerationPermissions(position)){
			createThreadModerationMenu(fcgiOut, data, subdatinTitle, threadId, position, getEffectiveUserPosition(data->con, userId, subdatinId), res->getBoolean("locked"), res->getBoolean("stickied"), flagContains(ThreadFlags::showDismissReport, flags));
		}
		fcgiOut << "<li>" << getFormattedThreadPostTime(data->con, threadId) << "</li>"
		"<li>" << getFormattedThreadBumpTime(data->con, threadId) << "</li>";
		if(res->getBoolean("locked")){
			fcgiOut << "<li>Locked</li>";
		}
		if(res->getBoolean("stickied")){
			fcgiOut << "<li>Stickied</li>";
		}
		fcgiOut << "</div>"
		"<div class='text'>"
		<< formatUserPostBody(data, res->getString("body"), hasRainbowTextPermissions(getEffectiveUserPosition(data->con, userId, subdatinId)))
		<< "</div>"
		"</div>";
	}
	
	return canReply;
}

void renderComment(MarkupOutStream& fcgiOut, RequestData* data, std::size_t commentId, bool isEven, bool canReply, UserPosition position, CommentFlags flags){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId, threadId, body, anonId, userId FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, commentId);
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	
	int64_t subdatinId = res->getInt64("subdatinId");
	std::string subdatinTitle = getSubdatinTitle(data->con, subdatinId);
	
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
	if(flagContains(CommentFlags::includeReplies, flags)){
		fcgiOut << "<a name='" << std::to_string(commentId) << "'></a>";
	}
	fcgiOut << (isEven?"<div class='comment even'>":"<div class='comment odd'>") << 
	"<div class='extraPostInfo'>";
	if(flagContains(CommentFlags::showSubdatin, flags)){
		fcgiOut << "<li><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "'>/" << subdatinTitle << "/</a></li>";
	}
	if(flagContains(CommentFlags::showPoster, flags)){
		fcgiOut << "<li>" << getFormattedPosterString(data->con, anonId, userId, subdatinId) << "</li>";
	}
	if(flagContains(CommentFlags::showReplyId, flags)){
		fcgiOut << "<li>"
		"<a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "'>" << std::to_string(commentId) << "</a>"
		"</li>";
	}
	if(!flagContains(CommentFlags::isPreview, flags)){
		if(canReply){
			createReplyMenu(fcgiOut, data, subdatinTitle, subdatinId, threadId, commentId);
		}
		createReportMenu(fcgiOut, data, subdatinTitle, threadId, commentId);
		if(hasModerationPermissions(position)){
			createCommentModerationMenu(fcgiOut, data, subdatinTitle, threadId, commentId, position, getEffectiveUserPosition(data->con, userId, subdatinId), flagContains(CommentFlags::showDismissReport, flags));
		}
	}
	
	fcgiOut << "<li>" << getFormattedCommentPostTime(data->con, commentId) << "</li>"
	"</div>";
	createPostImages(fcgiOut, data, -1, commentId, flagContains(CommentFlags::isPreview, flags));
	fcgiOut << "<div class='commentText'>" << formatUserPostBody(data, body, hasRainbowTextPermissions(getEffectiveUserPosition(data->con, userId, subdatinId)), flagContains(CommentFlags::includeReplies, flags)) << "</div>";
}

void createReportMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, int64_t commentId){
	fcgiOut <<
		"<li>"
		"<div class='dropDown'>"
		"<input type='checkbox' id='reportMenu" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
		"<label class='dropBtn' for='reportMenu" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
		"Report"
		"</label>"
		"<ul>";
	
	if(commentId == -1){
		fcgiOut <<
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/reportThread' class='inline'>"
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
			"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "/reportComment' class='inline'>"
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
		"</li>";
	
}

void createReplyMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t subdatinId, int64_t threadId, int64_t commentId){
	fcgiOut << "<li>"
	"<div class='dropDown'>"
	"<input type='checkbox' id='reply" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
	"<label class='dropBtn' for='reply" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
	"Reply"
	"</label>"
	"<ul>"
	"<li>"
	"Posting as " << getFormattedPosterString(data->con, data->shownId, data->userId, subdatinId, false) << "<br>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/newComment' accept-charset='UTF-8' enctype='multipart/form-data'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='hidden' name='parentId' value='" << std::to_string(commentId) << "'>"
	"<div class='subjectInput'><input type='text' name='subject'></div>"
	"<textarea name='body'></textarea><br>"
	"Image: <input type='file' name='postImage' accept='image/*'><br>"
	"Show Thumbnail?<input type='checkbox' name='showThumbnail' checked='checked' style='display: inline-block;'><br>"
	"<button type='submit' name='submit_param'>"
	"Create Comment"
	"</button>"
	"</li>"
	"</ul>"
	"</form>"
	"</div>"
	"</li>";
}

void createThreadModerationMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, UserPosition moderator, UserPosition poster, bool locked, bool stickied, bool showDismissReport){
	fcgiOut << "<li>"
	"<div class='dropDown'>"
	"<input type='checkbox' id='moderateMenu" << std::to_string(threadId) << "'>"
	"<label class='dropBtn' for='moderateMenu" << std::to_string(threadId)<< "'>"
	"Moderate"
	"</label>"
	"<ul>";
	if(hasModerationPermissionsOver(moderator, poster)){
		fcgiOut <<
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/deleteThread' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' class='link-button'>"
		"Delete"
		"</button>"
		"</form>"
		"</li>";
	}
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
	if(hasSubdatinControlPermissions(moderator)){
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
	"</li>";
	
	fcgiOut <<
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/hideThumbnails' class='inline'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<button type='submit' class='link-button'>"
	"Hide Image Thumbnails"
	"</button>"
	"</form>"
	"</li>";
	
	if(showDismissReport){
		fcgiOut << "<li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/dismissReports' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<input type='hidden' name='threadId' value='" << std::to_string(threadId) << "'>"
		"<input type='hidden' name='commentId' value='-1'>"
		"<button type='submit' class='link-button'>"
		"Dismiss Reports"
		"</button>"
		"</form></li>";
	}
	
	fcgiOut << "</ul>"
	"</div>"
	"</li>";
}

void createCommentModerationMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, int64_t commentId, UserPosition moderator, UserPosition poster, bool showDismissReport){
	fcgiOut << "<li>"
	"<div class='dropDown'>"
	"<input type='checkbox' id='moderationMenu" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
	"<label class='dropBtn' for='moderationMenu" << std::to_string(threadId) << "_" << std::to_string(commentId) << "'>"
	"Moderate"
	"</label>"
	"<ul>";
	if(hasModerationPermissionsOver(moderator, poster)){
		fcgiOut << 
		"<li>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "/deleteComment' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit' class='link-button'>"
		"Delete"
		"</button>"
		"</form>"
		"</li>";
	}
	
	fcgiOut <<
	"<li>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/thread/" << std::to_string(threadId) << "/comment/" << std::to_string(commentId) << "/hideThumbnails' class='inline'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<button type='submit' class='link-button'>"
	"Hide Image Thumbnails"
	"</button>"
	"</form>"
	"</li>";
	
	if(showDismissReport){
		fcgiOut << "<li><form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(subdatinTitle) << "/dismissReports' class='inline'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<input type='hidden' name='threadId' value='" << std::to_string(threadId) << "'>"
		"<input type='hidden' name='commentId' value='" << std::to_string(commentId) << "'>"
		"<button type='submit' class='link-button'>"
		"Dismiss Reports"
		"</button>"
		"</form></li>";
	}
	
	fcgiOut <<
	"</ul>"
	"</form>"
	"</div>"
	"</li>";
}

void createPostImages(MarkupOutStream& fcgiOut, RequestData* data, int64_t threadId, int64_t commentId, bool isPreview){
	std::unique_ptr<sql::PreparedStatement> prepStmt;
	if(threadId != -1){
		prepStmt.reset(data->con->prepareStatement("SELECT shownId, showThumbnail FROM images WHERE threadId = ?"));
		prepStmt->setInt64(1, threadId);
	}
	else{
		prepStmt.reset(data->con->prepareStatement("SELECT shownId, showThumbnail FROM images WHERE commentId = ?"));
		prepStmt->setInt64(1, commentId);
	}
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		fcgiOut << "<div class='postImage'>";
		if(isPreview){
			if(res->getBoolean("showThumbnail")){
				fcgiOut << "<img class='previewThumbnail' src='https://" << WebsiteFramework::getDomain() << "/postImageThumbnails/" << res->getString("shownId") << "'>";
			}
			else{
				fcgiOut << "<img class='previewThumbnail' src='https://" << WebsiteFramework::getDomain() << "/static/images/thumbnailHidden.png'>";
			}
		}
		else{
			fcgiOut << 
			"<input type='checkbox' id='image_" << res->getString("shownId") << "'>"
			"<label class='thumbnailLabel' for='image_" << res->getString("shownId") << "'>";
			if(res->getBoolean("showThumbnail")){
				fcgiOut << "<img class='thumbnail' src='https://" << WebsiteFramework::getDomain() << "/postImageThumbnails/" << res->getString("shownId") << "'>";
			}
			else{
				fcgiOut << "<img class='thumbnail' src='https://" << WebsiteFramework::getDomain() << "/static/images/thumbnailHidden.png'>";
			}
			fcgiOut << "</label>"
			"<label class='fullImageLabel' for='image_" << res->getString("shownId") << "'>"
			"<img class='fullImage' src='https://" << WebsiteFramework::getDomain() << "/postImages/" << res->getString("shownId") << "'>"
			"</label>";
		}
		fcgiOut << "</div>";
	}
}