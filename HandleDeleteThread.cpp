#include "HandleDeleteThread.h"

void handleDeleteThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
		
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		handleDeleteThreadErrorPage(fcgi, data, "You Do Not Have The Correct Permissions To Do This");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("DELETE FROM threads WHERE id = ?"));
	prepStmtB->setInt64(1, data->threadId);
	prepStmtB->execute();
	
	std::string seeAlso;
	InputError error = getPostValue(fcgi->cgi, seeAlso, "seeAlso", Config::getUniqueTokenLength(), InputFlag::AllowNonNormal);
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	if(error != InputError::NoError || seeAlso.size() == 0){
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0]);
	}
	else{
		sendLocationHeader(fcgi->out, seeAlso);
	}
	finishHttpHeader(fcgi->out);
}

void handleDeleteThreadErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}