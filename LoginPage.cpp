#include "LoginPage.h"

void createLoginPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createLoginPage(fcgi, data, "", "");
	}
	else{
		sendStatusHeader(fcgi->out, StatusCode::SeeOther);
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/");
		finishHttpHeader(fcgi->out);
	}
}

void createLoginPage(FcgiData* fcgi, RequestData* data, std::string loginError, std::string createAccountError){
	createPageHeader(fcgi, data);
	
	fcgi->out << 
	"<h1>Login:</h1>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/login' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='userName'>Username<br>"
	"<input type='password' name='password'>Password<br>"
	;
	if(loginError != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< loginError <<
		"</div></p>";
	}
	fcgi->out <<
	"<button type='submit' name='submit_param'>"
	"Login"
	"</button>"
	"</form>"
	"<h1>Create Account:</h1>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/createAccount' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='userName'>Username<br>"
	"<input type='password' name='password'>Password<br>"
	"<input type='password' name='repeatPassword'>Repeat Password<br>"
	"<script src='https://www.google.com/recaptcha/api.js'></script>"
	"<div class='g-recaptcha' data-sitekey='6LeYjR4UAAAAAExPuz3j60KPeea5jZ3zvd9KKR8b'></div>"
	;
	if(createAccountError != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< createAccountError <<
		"</div></p>";
	}
	fcgi->out <<
	"<button type='submit'>"
	"Create Account"
	"</button>"
	"</form>";
	
	createPageFooter(fcgi, data);
}
