#include "HandleNewComment.h"

void handleNewComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string title;
	std::string name;
	bool postLocked;
	bool commentLocked;
	getSubdatinData(data->con, data->subdatinId, title, name, postLocked, commentLocked);
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId)) && commentLocked){
		createGenericErrorPage(fcgi, data, "You cannot post a comment in a subdatin with locked comments");
		return;
	}
	
	if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonPostingTimeout()) || 
		(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userPostingTimeout())){
		createGenericErrorPage(fcgi, data, "You are posting/reporting too much, wait a little longer before trying again");
		return;
	}
	
	std::string body;
	switch(getPostValue(fcgi->cgi, body, "body", Config::getMaxPostLength(), InputFlag::AllowNonNormal | InputFlag::AllowNewLine)){
	default:
		createGenericErrorPage(fcgi, data, "Unknown Comment Error");
		return;
	case InputError::IsTooLarge:
		createGenericErrorPage(fcgi, data, "Comment Too Long");
		return;
	case InputError::IsEmpty:
		createGenericErrorPage(fcgi, data, "Comments Cannot Be Blank");
		return;
	case InputError::IsMalformed:
		createGenericErrorPage(fcgi, data, "Comment Contains Invalid Characters");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createGenericErrorPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	std::string parentId;
	if(getPostValue(fcgi->cgi, parentId, "parentId", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError){
		createGenericErrorPage(fcgi, data, "Invalid Parent Comment Id");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT locked FROM threads WHERE id = ? AND subdatinId = ?"));
	prepStmt->setInt64(1, data->threadId);
	prepStmt->setInt64(2, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(!res->next()){
		createGenericErrorPage(fcgi, data, "Cannot Reply In A Thread That Doesn't Exist");
		return;
	}
	bool threadLocked = res->getBoolean("locked");
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId)) && threadLocked){
		createGenericErrorPage(fcgi, data, "Cannot Reply In a Thread With Comments Locked");
		return;
	}
	
	if(parentId != "-1"){
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM comments WHERE id = ? AND threadId = ? AND subdatinId = ?"));
		prepStmt->setString(1, parentId);
		prepStmt->setInt64(2, data->threadId);
		prepStmt->setInt64(3, data->subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(!res->next()){
			createGenericErrorPage(fcgi, data, "Cannot Reply To A Comment That Doesn't Exist");
			return;
		}
	}
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement(
		"INSERT INTO comments (body, anonId, userId, posterIp, threadId, subdatinId, parentId) VALUES (?, ?, ?, ?, ?, ?, ?)"));
	
	prepStmt->setString(1, body);
	if(data->userId == -1){
		prepStmt->setString(2, data->shownId);
		prepStmt->setNull(3, 0);
	}
	else{
		prepStmt->setNull(2, 0);
		prepStmt->setInt64(3, data->userId);
	}
	prepStmt->setString(4, fcgi->env->getRemoteAddr());
	prepStmt->setInt64(5, data->threadId);
	prepStmt->setInt64(6, data->subdatinId);
	if(parentId == "-1"){
		prepStmt->setNull(7, 0);
	}
	else{
		prepStmt->setString(7, parentId);
	}
	
	prepStmt->execute();
	
	if(parentId == "-1"){//if we should bump the thread
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("UPDATE threads SET lastBumpTime = CURRENT_TIMESTAMP WHERE id = ?"));
		prepStmt->setInt64(1, data->threadId);
		prepStmt->execute();
	}
	
	setLastPostTime(fcgi, data);
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
	finishHttpHeader(fcgi->out);
}





























