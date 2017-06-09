#include "HandleSetCssTheme.h"

void handleSetCssTheme(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createGenericErrorPage(fcgi, data, "You Must Be LoggedIn In Order To Complete This Action");
		return;
	}
	
	std::string theme;
	switch(getPostValue(fcgi->cgi, theme, "theme", Config::getUniqueTokenLength(), InputFlag::AllowStrictOnly)){
	default:
		createSettingsPage(fcgi, data, "Invalid Theme", "");
		return;
	case InputError::NoError:
		break;
	}
	
	if(theme != "dark" && theme != "light" && theme != "aesthicc" && theme != "anime" && theme != "synthwave"){
		createSettingsPage(fcgi, data, "Invalid Theme", "");
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE users SET cssTheme=? WHERE id=?"));
	prepStmt->setString(1, theme);
	prepStmt->setInt64(2, data->userId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
