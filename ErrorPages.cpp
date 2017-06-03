#include "ErrorPages.h"

void createGenericErrorPage(FcgiData* fcgi, RequestData* data, std::string message){
	createPageHeader(fcgi, data, PageTab::Error);
	fcgi->out << "<div class='errorText'>" << message << "</div>";
	createPageFooter(fcgi, data);
}

void createInvalidPermissionsErrorPage(FcgiData* fcgi, RequestData* data){
	createPageHeader(fcgi, data, PageTab::Error);
	fcgi->out << "<div class='errorText'>You do not have the valid permissions to view this page/complete this action.</div>";
	createPageFooter(fcgi, data);
}