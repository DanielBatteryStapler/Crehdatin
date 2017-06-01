#include "HandleSetThreadStickied.h"

void handleSetThreadStickied(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		handleSetThreadStickiedErrorPage(fcgi, data, "You Do Not Have The Correct Permissions To Do This");
		return;
	}
	
	std::string stickied;
	if(getPostValue(fcgi->cgi, stickied, "stickied", 10, InputFlag::AllowStrictOnly) != InputError::NoError){
		handleSetThreadStickiedErrorPage(fcgi, data, "Invalid 'stickied' Valid");
		return;
	}
	
	bool stickiedBool;
	if(stickied == "true"){
		stickiedBool = true;
	}
	else if(stickied == "false"){
		stickiedBool = false;
	}
	else{
		handleSetThreadStickiedErrorPage(fcgi, data, "Invalid 'stickied' Valid");
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("UPDATE threads SET stickied = ? WHERE id = ?"));
	prepStmtB->setBoolean(1, stickiedBool);
	prepStmtB->setInt64(2, data->threadId);
	prepStmtB->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
	finishHttpHeader(fcgi->out);
}

void handleSetThreadStickiedErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}