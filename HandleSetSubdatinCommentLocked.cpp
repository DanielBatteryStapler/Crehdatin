#include "HandleSetSubdatinCommentLocked.h"

void handleSetSubdatinCommentLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string locked;
	if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		createGenericErrorPage(fcgi, data, "Invalid 'locked' Value");
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
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE subdatins SET commentLocked = ? WHERE id = ?"));
	prepStmt->setBoolean(1, lockedBool);
	prepStmt->setInt64(2, data->subdatinId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/controlPanel");
	finishHttpHeader(fcgi->out);
}









