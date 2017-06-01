#include "HandleSetSubdatinPostLocked.h"

void handleSetSubdatinPostLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createSetSubdatinPostLockedErrorPage(fcgi, data, "You do not have the correct permissions to do this");
		return;
	}
	
	std::string locked;
	if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		createSetSubdatinPostLockedErrorPage(fcgi, data, "Invalid 'Locked' Valid");
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
		createSetSubdatinPostLockedErrorPage(fcgi, data, "Invalid 'Locked' Valid");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE subdatins SET postLocked = ? WHERE id = ?"));
	prepStmt->setBoolean(1, lockedBool);
	prepStmt->setInt64(2, data->subdatinId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/controlPanel");
	finishHttpHeader(fcgi->out);
}

void createSetSubdatinPostLockedErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}









