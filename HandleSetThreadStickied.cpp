#include "HandleSetThreadStickied.h"

void handleSetThreadStickied(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		handleSetThreadStickiedErrorPage(fcgi, data, -1, "Cannot set a thread stickied in a subdatin that doesn't exist");
	}
	else{
		std::string authToken;
		if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
			|| authToken != data->authToken){
			handleSetThreadStickiedErrorPage(fcgi, data, subdatinId, "Invalid Authentication Token");
			return;
		}
		
		if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			handleSetThreadStickiedErrorPage(fcgi, data, subdatinId, "You Do Not Have The Correct Permissions To Do This");
			return;
		}
		
		std::string stickied;
		if(getPostValue(fcgi->cgi, stickied, "stickied", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
			handleSetThreadStickiedErrorPage(fcgi, data, subdatinId, "Invalid 'stickied' Valid");
			return;
		}
		
		bool stickiedBool;
		if(stickied == "true"){
			stickiedBool = true;
		}
		else if(stickied == "false"){
			stickiedBool = false;
		}
		else{
			handleSetThreadStickiedErrorPage(fcgi, data, subdatinId, "Invalid 'stickied' Valid");
			return;
		}
		
		std::string threadId = percentDecode(parameters[1]);
		
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM threads WHERE id = ? AND subdatinId = ?"));
		prepStmt->setString(1, threadId);
		prepStmt->setInt64(2, subdatinId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		
		if(res->next()){
			std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("UPDATE threads SET stickied = ? WHERE id = ?"));
			prepStmtB->setBoolean(1, stickiedBool);
			prepStmtB->setString(2, threadId);
			prepStmtB->execute();
			
			sendStatusHeader(fcgi->out, StatusCode::SeeOther);
			sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
			finishHttpHeader(fcgi->out);
		}
		else{
			handleSetThreadStickiedErrorPage(fcgi, data, subdatinId, "This Thread Does Not Exist");
		}
	}
}

void handleSetThreadStickiedErrorPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string error){
	createPageHeader(fcgi, data, subdatinId);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}