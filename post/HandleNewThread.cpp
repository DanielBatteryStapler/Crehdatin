#include "HandleNewThread.h"

void handleNewThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	bool postLocked;
	bool commentLocked;
	getSubdatinLockedData(data->con, data->subdatinId, postLocked, commentLocked);
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId)) && postLocked){
		createNewThreadPage(fcgi, data, "You cannot post in a subdatin with posts locked.");
		return;
	}
	
	if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonPostingTimeout()) || 
		(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userPostingTimeout())){
		createNewThreadPage(fcgi, data, "You are posting too much, wait a little longer before trying again");
		return;
	}
	
	std::string title;
	switch(getPostValue(fcgi->cgi, title, "title", Config::getMaxTitleLength(), InputFlag::AllowNonNormal)){
	default:
		createNewThreadPage(fcgi, data, "Unknown Title Error");
		return;
	case InputError::IsTooLarge:
		createNewThreadPage(fcgi, data, "Title Too Long");
		return;
	case InputError::IsEmpty:
		createNewThreadPage(fcgi, data, "Titles Cannot Be Blank");
		return;
	case InputError::ContainsNewLine: case InputError::IsMalformed:
		createNewThreadPage(fcgi, data, "Titles Cannot Contain Newlines");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string body;
	switch(getPostValue(fcgi->cgi, body, "body", Config::getMaxPostLength(), InputFlag::AllowNonNormal | InputFlag::AllowNewLine)){
	default:
		createNewThreadPage(fcgi, data, "Unknown Post Error");
		return;
	case InputError::IsTooLarge:
		createNewThreadPage(fcgi, data, "Post Too Long");
		return;
	case InputError::IsEmpty:
		createNewThreadPage(fcgi, data, "Posts Cannot Be Blank");
		return;
	case InputError::IsMalformed:
		createNewThreadPage(fcgi, data, "Post Contains Invalid Characters");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createNewThreadPage(fcgi, data, "Invalid Authentication Token");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement(
		"INSERT INTO threads (title, body, anonId, userId, subdatinId, posterIp, lastBumpTime) VALUES (?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP)"));
	
	prepStmt->setString(1, title);
	prepStmt->setString(2, body);
	if(data->userId == -1){
		prepStmt->setString(3, data->shownId);
		prepStmt->setNull(4, 0);
	}
	else{
		prepStmt->setNull(3, 0);
		prepStmt->setInt64(4, data->userId);
	}
	prepStmt->setInt64(5, data->subdatinId);
	prepStmt->setString(6, fcgi->env->getRemoteAddr());
	
	prepStmt->execute();
	
	setLastPostTime(fcgi, data);
	
	std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT LAST_INSERT_ID()"));
	res->first();
	int newThreadId = res->getInt64(1);
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + percentEncode(parameters[0]) + "/thread/" + std::to_string(newThreadId));
	finishHttpHeader(fcgi->out);
}












