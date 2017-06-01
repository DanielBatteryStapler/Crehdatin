#include "HandleSetThreadLocked.h"

void handleSetThreadLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
		
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		handleSetThreadLockedErrorPage(fcgi, data, "You Do Not Have The Correct Permissions To Do This");
		return;
	}
	
	std::string locked;
	if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		handleSetThreadLockedErrorPage(fcgi, data, "Invalid 'Locked' Valid");
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
		handleSetThreadLockedErrorPage(fcgi, data, "Invalid 'Locked' Valid");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("UPDATE threads SET locked = ? WHERE id = ?"));
	prepStmtB->setBoolean(1, lockedBool);
	prepStmtB->setInt64(2, data->threadId);
	prepStmtB->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
	finishHttpHeader(fcgi->out);
}

void handleSetThreadLockedErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}