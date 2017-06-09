#include "HandleAddAdministrator.h"

void handleAddAdministrator(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string userName;
	switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createGenericErrorPage(fcgi, data, "Invalid Username");
		return;
	case InputError::NoError:
		break;
	}
	
	int64_t userId = getUserId(data->con, userName);
	
	if(userId == -1){
		createGenericErrorPage(fcgi, data, "The Specified User Does Not Exist");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE users SET userPosition = 'administrator' WHERE id = ?"));
	prepStmt->setInt64(1, userId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}









