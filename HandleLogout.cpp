#include "HandleLogin.h"

void handleLogout(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/");
		finishHttpHeader(fcgi->out);
		return;
	}
	
	std::string shownId = generateRandomToken().substr(0, 12);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE sessions SET userId=NULL, shownId=? WHERE sessionToken=?"));
	prepStmt->setString(1, shownId);
	prepStmt->setString(2, data->sessionToken);
	prepStmt->execute();
	
	std::string seeOther;
	InputError error = getPostValue(fcgi->cgi, seeOther, "seeOther", Config::getUniqueTokenLength() * 2, InputFlag::AllowNonNormal);
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	if(error != InputError::NoError || seeOther.size() == 0){
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/");
	}
	else{
		sendLocationHeader(fcgi->out, seeOther);
	}
	finishHttpHeader(fcgi->out);
}












