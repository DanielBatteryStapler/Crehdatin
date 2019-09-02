#include "HandleSetThreadStickied.h"

void handleSetThreadStickied(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string stickied;
	if(getPostValue(fcgi->cgi, stickied, "stickied", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		createGenericErrorPage(fcgi, data, "Invalid 'stickied' Valid");
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
		createGenericErrorPage(fcgi, data, "Invalid 'stickied' Valid");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("UPDATE threads SET stickied = ? WHERE id = ?"));
	prepStmtB->setBoolean(1, stickiedBool);
	prepStmtB->setInt64(2, data->threadId);
	prepStmtB->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
