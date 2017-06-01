#include "HandleSetSubdatinCommentLocked.h"

void handleSetSubdatinCommentLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createSetSubdatinCommentLockedErrorPage(fcgi, data, "You do not have the correct permissions to do this");
		return;
	}
	
	std::string locked;
	if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		createSetSubdatinCommentLockedErrorPage(fcgi, data, "Invalid 'Locked' Valid");
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
		createSetSubdatinCommentLockedErrorPage(fcgi, data, "Invalid 'Locked' Valid");
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

void createSetSubdatinCommentLockedErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}









