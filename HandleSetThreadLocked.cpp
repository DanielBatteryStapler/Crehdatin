#include "HandleSetThreadLocked.h"

void handleSetThreadLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		handleSetThreadLockedErrorPage(fcgi, data, -1, "Cannot set a thread locked in a subdatin that doesn't exist");
	}
	else{
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			handleSetThreadLockedErrorPage(fcgi, data, subdatinId, "Invalid Authentication Token");
			return;
		}
		
		if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			handleSetThreadLockedErrorPage(fcgi, data, subdatinId, "You Do Not Have The Correct Permissions To Do This");
			return;
		}
		
		std::string locked;
		if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
			handleSetThreadLockedErrorPage(fcgi, data, subdatinId, "Invalid 'Locked' Valid");
			return;
		}
		
		bool lockedBool;
		if(locked == "true"){
			lockedBool = true;
		}
		else if(locked == "false"){
			lockedBool = false;
		}
		else{
			handleSetThreadLockedErrorPage(fcgi, data, subdatinId, "Invalid 'Locked' Valid");
			return;
		}
		
		std::string threadId = percentDecode(parameters[1]);
		
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM threads WHERE id = ? AND subdatinId = ?"));
		prepStmt->setString(1, threadId);
		prepStmt->setInt64(2, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(res->next()){
			std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("UPDATE threads SET locked = ? WHERE id = ?"));
			prepStmtB->setBoolean(1, lockedBool);
			prepStmtB->setString(2, threadId);
			prepStmtB->execute();
			
			sendStatusHeader(fcgi->out, StatusCode::SeeOther);
			sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
			finishHttpHeader(fcgi->out);
		}
		else{
			handleSetThreadLockedErrorPage(fcgi, data, subdatinId, "This Thread Does Not Exist");
		}
	}
}

void handleSetThreadLockedErrorPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string error){
	createPageHeader(fcgi, data, subdatinId);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}