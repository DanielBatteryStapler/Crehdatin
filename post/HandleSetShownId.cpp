#include "HandleSetShownId.h"

void handleSetShownId(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createMustBeLoggedInErrorPage(fcgi, data);
		return;
	}
	
	std::string anonFlag;
	switch(getPostValue(fcgi->cgi, anonFlag, "anonFlag", 1, InputFlag::AllowStrictOnly)){
	default:
		createGenericErrorPage(fcgi, data, "Invalid Anon Flag");
		return;
	case InputError::NoError:
		break;
	}
	
	if(anonFlag == "0"){
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE users SET shownId=NULL WHERE id=?"));
		prepStmt->setInt64(1, data->userId);
		prepStmt->execute();
	}
	else if(anonFlag == "1"){
		data->shownId = generateRandomToken();
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE users SET shownId=? WHERE id=?"));
		prepStmt->setString(1, data->shownId);
		prepStmt->setInt64(2, data->userId);
		prepStmt->execute();
	}
	else{
		createGenericErrorPage(fcgi, data, "Invalid Anon Flag");
		return;
	}
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
