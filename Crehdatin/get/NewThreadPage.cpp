#include "NewThreadPage.h"

void createNewThreadPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createNewThreadPage(fcgi, data, "");
}

void createNewThreadPage(FcgiData* fcgi, RequestData* data, std::string error){
	std::string title = getSubdatinTitle(data->con, data->subdatinId);
	
	createPageHeader(fcgi, data, PageTab::NewThread);
	fcgi->out << "<h1>New Thread</h1>"
	"Posting to /" << title << "/ as " << getFormattedPosterString(data->con, data->shownId, data->userId, data->subdatinId, false) << "<br>"
	"<form method='post' action='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(title) << "/newThread' accept-charset='UTF-8' enctype='multipart/form-data'>"
	"<input type='hidden' name='authToken' value='" << data->authToken << "'>"
	"Title: <input type='text' name='title'><br>"
	"<textarea name='body'></textarea><br>"
	"Image: <input type='file' name='postImage' accept='image/*'><br>"
	"Show Thumbnail?<input type='checkbox' name='showThumbnail' checked='checked' style='display: inline-block;'><br>"
	"<div class='subjectInput'><input type='text' name='subject'></div>";
	if(error != ""){
		fcgi->out << "<p><div class='errorText'>" << error << "</div></p>";
	}
	fcgi->out <<
	"<button type='submit'>"
	"Create Thread"
	"</button>"
	"</form>";
	
	createPageFooter(fcgi, data);
}