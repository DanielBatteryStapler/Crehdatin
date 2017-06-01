#include "HandleSetModerator.h"

void handleSetModerator(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string authToken;
	if(getPostValue(fcgi->cgi, authToken, "authToken", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly) != InputError::NoError 
		|| authToken != data->authToken){
		createSetModeratorErrorPage(fcgi, data, -1, "Invalid Authentication Token");
		return;
	}
	
	int64_t subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(subdatinId == -1){
		createSetModeratorErrorPage(fcgi, data, -1, "Cannot edit a subdatin that doesn't exist");
	}
	else{
		if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, subdatinId))){
			createSetModeratorErrorPage(fcgi, data, subdatinId, "You do not have the correct permissions to do this");
			return;
		}
		
		std::string userName;
		switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
		default:
			createSetModeratorErrorPage(fcgi, data, subdatinId, "Unknown Username Error");
			return;
		case InputError::IsTooLarge:
			createSetModeratorErrorPage(fcgi, data, subdatinId, "Username Too Long");
			return;
		case InputError::IsEmpty:
			createSetModeratorErrorPage(fcgi, data, subdatinId, "Username Cannot Be Empty");
			return;
		case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
			createSetModeratorErrorPage(fcgi, data, subdatinId, "Username Can Only Contain Letters, Numbers, Hyphen, and Underscore");
			return;
		case InputError::NoError:
			break;
		}
		
		int64_t userId = getUserId(data->con, userName);
		
		if(userId == -1){
			createSetModeratorErrorPage(fcgi, data, subdatinId, "The specified user does not exist");
			return;
		}
		
		std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE userPositions SET userPosition = 'moderator' WHERE userId = ? AND subdatinId = ?"));
		prepStmt->setInt64(1, userId);
		prepStmt->setInt64(2, subdatinId);
		prepStmt->execute();
		
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + Config::getDomain() + "/d/" + parameters[0] + "/controlPanel");
		finishHttpHeader(fcgi->out);
	}
}

void createSetModeratorErrorPage(FcgiData* fcgi, RequestData* data, int64_t subdatinId, std::string error){
	createPageHeader(fcgi, data, subdatinId);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}









