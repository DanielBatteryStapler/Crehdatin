#include "HandleSetCssTheme.h"

void handleSetCssTheme(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createPageHeader(fcgi, data);
		fcgi->out << 
		"<p><div class='errorText'>"
		"You can only do this if you are logged in."
		"</div></p>";
		createPageFooter(fcgi, data);
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
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/settings");
	finishHttpHeader(fcgi->out);
}
