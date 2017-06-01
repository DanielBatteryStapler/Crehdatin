#include "HandleRemoveSubdatinOfficial.h"

void handleRemoveSubdatinOfficial(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createRemoveSubdatinOfficialErrorPage(fcgi, data, "You do not have the correct permissions to do this");
		return;
	}
	
	std::string userName;
	switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createRemoveSubdatinOfficialErrorPage(fcgi, data, "Unknown Username Error");
		return;
	case InputError::IsTooLarge:
		createRemoveSubdatinOfficialErrorPage(fcgi, data, "Username Too Long");
		return;
	case InputError::IsEmpty:
		createRemoveSubdatinOfficialErrorPage(fcgi, data, "Username Cannot Be Empty");
		return;
	case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
		createRemoveSubdatinOfficialErrorPage(fcgi, data, "Username Can Only Contain Letters, Numbers, Hyphen, and Underscore");
		return;
	case InputError::NoError:
		break;
	}
	
	int64_t userId = getUserId(data->con, userName);
	
	if(userId == -1){
		createRemoveSubdatinOfficialErrorPage(fcgi, data, "The specified user does not exist");
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

void createRemoveSubdatinOfficialErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}









