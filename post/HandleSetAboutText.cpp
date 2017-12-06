#include "HandleSetAboutText.h"

void handleSetAboutText(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasSubdatinControlPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string text;
	switch(getPostValue(fcgi->cgi, text, "text", Config::getMaxPostLength(), InputFlag::AllowNonNormal | InputFlag::AllowNewLine)){
	default:
		createGenericErrorPage(fcgi, data, "Unknown Text Error");
		return;
	case InputError::IsTooLarge:
		createGenericErrorPage(fcgi, data, "Text Too Long");
		return;
	case InputError::IsEmpty:
		text.clear();
		return;
	case InputError::IsMalformed:
		createGenericErrorPage(fcgi, data, "Text Contains Invalid Characters");
		return;
	case InputError::NoError:
		break;
	}
	
	if(text.size() == 0){
		std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE subdatins SET description = NULL WHERE id = ?"));
		prepStmt->setInt64(1, data->subdatinId);
		prepStmt->execute();
	}
	else{
		std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("UPDATE subdatins SET description = ? WHERE id = ?"));
		prepStmt->setString(1, text);
		prepStmt->setInt64(2, data->subdatinId);
		prepStmt->execute();
	}
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
