#include "LoginPage.h"

void createLogoutPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/");
		finishHttpHeader(fcgi->out);
	}
	else{
		createPageHeader(fcgi, data, PageTab::Logout);
		
		fcgi->out << 
		"<h1>Are You Sure?</h1>"
		"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/logout' accept-charset='UTF-8'>"
		"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
		"<button type='submit'>"
		"Logout"
		"</button>"
		"</form>";
		
		createPageFooter(fcgi, data);
	}
}
