#include "UserPage.h"

void createUserPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::User);
	
	
	
	createPageFooter(fcgi, data);
}
