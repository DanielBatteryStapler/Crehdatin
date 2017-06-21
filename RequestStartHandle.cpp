#include "RequestStartHandle.h"

bool requestStartHandle(FcgiData* fcgi, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT UNIX_TIMESTAMP(CURRENT_TIMESTAMP)"));
	
	res->first();
	
	data->sessionToken = "";
	data->captchaCode = "";
	data->captchaSeed = -1;
	data->userName = "";
	data->userId = -1;
	data->authToken = "";
	data->shownId = "";
	data->cssTheme = "dark";
	data->blocked = false;
	data->lastPostTime = 0;
	
	data->currentTime = res->getInt64(1);
	data->subdatinId = -1;
	data->threadId = -1;
	data->commentId = -1;
	data->userPageId = -1;
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT " 
	"blocked, UNIX_TIMESTAMP(lastPostTime) "
	"FROM ips "
	"WHERE ip = ?"));
	prepStmt->setString(1, fcgi->env->getRemoteAddr());
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(res->next()){
		data->blocked = res->getBoolean("blocked");
		data->lastPostTime = res->getInt64("UNIX_TIMESTAMP(lastPostTime)");
	}
	
	if(data->blocked){
		sendStatusHeader(fcgi->out, StatusCode::Ok);
		sendContentTypeHeader(fcgi->out, ContentType::Html);
		finishHttpHeader(fcgi->out);
		fcgi->out << "<!DOCTYPE html><html><body>You are blocked</body></html>";
		return false;
	}
	
	std::vector<cgicc::HTTPCookie> cookies = fcgi->env->getCookieList();
	std::string sessionToken;
	for(auto i = cookies.begin(); i != cookies.end(); i++){
		if(i->getName() == "sessionToken"){
			sessionToken = i->getValue();
			break;
		}
	}
	
	if(getSessionData(fcgi, data, sessionToken) == false){
		return false;
	}
	
	if(fcgi->env->getRequestMethod() == "POST"){//if you are trying to do a post request, you must have a correct auth token
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			createGenericErrorPage(fcgi, data, "Invalid Authentication Token. Do You Have Cookies Enabled?");
			return false;
		}
		std::string honeyPot;//just to stop web crawlers from spamming
		if(getPostValue(fcgi->cgi, honeyPot, "subject", 256, InputFlag::DontCheckInputContents) != InputError::IsEmpty || honeyPot != ""){
			createGenericErrorPage(fcgi, data, "Never Fill Out Subject Lines");
			return false;
		}
	}
	
	return true;
}

bool getSessionData(FcgiData* fcgi, RequestData* data, std::string sessionToken){
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT " 
	"captchaCode, captchaSeed, userId, authToken, shownId "
	"FROM sessions "
	"WHERE sessionToken = ?"));
	prepStmt->setString(1, sessionToken);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		data->sessionToken = sessionToken;
		data->captchaCode = res->getString("captchaCode");
		data->captchaSeed = res->getInt64("captchaSeed");
		if(!res->isNull("userId")){
			data->userId = res->getInt64("userId");
			
			std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("SELECT " 
			"userName, cssTheme, UNIX_TIMESTAMP(lastPostTime) "
			"FROM users "
			"WHERE id = ?"));
			
			prepStmtB->setInt64(1, data->userId);
			
			std::unique_ptr<sql::ResultSet> resB(prepStmtB->executeQuery());
			
			resB->beforeFirst();
			
			if(resB->next()){
				data->userName = resB->getString("userName");
				
				if(resB->isNull("cssTheme") == false){
					data->cssTheme = resB->getString("cssTheme");
				}
				
				if(resB->isNull("UNIX_TIMESTAMP(lastPostTime)") == false){
					data->lastPostTime = resB->getInt64("UNIX_TIMESTAMP(lastPostTime)");
				}
			}
			else{
				sendStatusHeader(fcgi->out, StatusCode::Ok);
				sendContentTypeHeader(fcgi->out, ContentType::Html);
				sendDeleteCookieHeader(fcgi->out, "sessionToken");
				finishHttpHeader(fcgi->out);
				fcgi->out << "<html><body>An unknown, internal server error occurred</body></html>";
				return false;
			}
		}
		data->authToken = res->getString("authToken");
		if(!res->isNull("shownId")){
			data->shownId = res->getString("shownId");
		}
	}
	else{
		createNewSession(fcgi, data);
		sendCookieHeader(fcgi->out, "sessionToken", data->sessionToken);
	}
	return true;
}

void createNewSession(FcgiData* fcgi, RequestData* data){
	data->stmt->execute("BEGIN");
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("INSERT INTO sessions "
	"(sessionToken, authToken, shownId, ip) SELECT ?, ?, ?, ? FROM DUAL WHERE NOT EXISTS "
	"(SELECT id FROM sessions WHERE sessionToken=? OR authToken=? OR shownId=?)"));
	
	int64_t rowCount;
	
	while(true){
		data->sessionToken = generateRandomToken();
		data->authToken = generateRandomToken();
		data->shownId = generateRandomToken().substr(0, 12);
		data->captchaCode = generateCaptchaText();
		data->captchaSeed = generateCaptchaSeed();
		
		prepStmt->setString(1, data->sessionToken);
		prepStmt->setString(2, data->authToken);
		prepStmt->setString(3, data->shownId);
		
		prepStmt->setString(4, fcgi->env->getRemoteAddr());
		
		prepStmt->setString(5, data->sessionToken);
		prepStmt->setString(6, data->authToken);
		prepStmt->setString(7, data->shownId);
		prepStmt->execute();
		
		std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT ROW_COUNT()"));
		
		res->first();
		
		rowCount = res->getInt64(1);
		if(rowCount == 0){
			data->stmt->execute("ROLLBACK");
			continue;
		}
		else{
			data->stmt->execute("COMMIT");
			break;
		}
	}
	
	createNewCaptchaSession(data);
}