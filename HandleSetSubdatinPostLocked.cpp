#include "HandleSetSubdatinPostLocked.h"

void handleSetSubdatinPostLocked(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createSetSubdatinPostLockedErrorPage(fcgi, data, -1, "Invalid Authentication Token");
		return;
	}
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		createSetSubdatinPostLockedErrorPage(fcgi, data, -1, "Cannot edit a subdatin that doesn't exist");
	}
	else{
		if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			createSetSubdatinPostLockedErrorPage(fcgi, data, subdatinId, "You do not have the correct permissions to do this");
			return;
		}
		
		std::string locked;
		if(getPostValue(fcgi->cgi, locked, "locked", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
			createSetSubdatinPostLockedErrorPage(fcgi, data, subdatinId, "Invalid 'Locked' Valid");
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
			createSetSubdatinPostLockedErrorPage(fcgi, data, subdatinId, "Invalid 'Locked' Valid");
			return;
		}
		
		std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE subdatins SET postLocked = ? WHERE id = ?"));
		prepStmt->setBoolean(1, lockedBool);
		prepStmt->setInt64(2, subdatinId);
		prepStmt->execute();
		
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/d/" + parameters[0] + "/controlPanel");
		finishHttpHeader(fcgi->out);
	}
}

void createSetSubdatinPostLockedErrorPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string error){
	createPageHeader(fcgi, data, subdatinId);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}









