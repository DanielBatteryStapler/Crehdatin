#include "HandleSetThreadLocked.h"

void handleSetThreadLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
		
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string locked;
	if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		createGenericErrorPage(fcgi, data, "Invalid 'locked' Valid");
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
		createGenericErrorPage(fcgi, data, "Invalid 'locked' Valid");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("UPDATE threads SET locked = ? WHERE id = ?"));
	prepStmtB->setBoolean(1, lockedBool);
	prepStmtB->setInt64(2, data->threadId);
	prepStmtB->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}