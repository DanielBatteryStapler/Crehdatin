#include "HandleDeleteComment.h"

void handleDeleteComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("DELETE FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, data->commentId);
	prepStmt->execute();
	
	std::string seeOther;
	InputError error = getPostValue(fcgi->cgi, seeOther, "seeOther", Config::getUniqueTokenLength(), InputFlag::AllowNonNormal);
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	if(error != InputError::NoError || seeOther.size() == 0){
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
	}
	else{
		sendLocationHeader(fcgi->out, seeOther);
	}
	finishHttpHeader(fcgi->out);
}