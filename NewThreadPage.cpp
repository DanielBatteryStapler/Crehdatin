#include "NewThreadPage.h"

void createNewThreadPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'><i>This subdatin does not exist...</i></div>";
	createPageFooter(fcgi, data);
}

void createNewThreadPage(FcgiData* fcgi, RequestData* data, std::string error){
	std::string title;
	std::string name;
	bool postLocked;
	bool commentLocked;
	
	getSubdatinData(data->con, data->subdatinId, title, name, postLocked, commentLocked);
	
	createPageHeader(fcgi, data);
	fcgi->out << "<h1>New Thread</h1>"
	"<h3>Posting To " << escapeHtml(name) << "</h3>"
	"<h3>Posting with " << getFormattedPosterString(data->con, data->shownId, data->userId) << "</h3>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(title) << "/newThread' accept-charset='UTF-8'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"<input type='text' name='title'>Title<br>"
	"<textarea name='body'></textarea><br>";
	if(error != ""){
		fcgi->out << 
		"<p><div class='errorText'>"
		<< error <<
		"</div></p>";
	}
	fcgi->out <<
	"<button type='submit' name='submit_param'>"
	"Create Thread"
	"</button>"
	"</form>";
	
	createPageFooter(fcgi, data);
}