#include "HandleRemoveSubdatinOfficial.h"

void handleRemoveSubdatinOfficial(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
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
		createGenericErrorPage(fcgi, data, "The specified user does not exist");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("DELETE FROM userPositions WHERE userId = ? AND subdatinId = ?"));
	prepStmt->setInt64(1, userId);
	prepStmt->setInt64(2, data->subdatinId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/controlPanel");
	finishHttpHeader(fcgi->out);
}









