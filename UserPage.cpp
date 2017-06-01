#include "UserPage.h"

void createUserPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string userName = percentDecode(parameters[0]);
	int64_t userId = getUserId(data->con, userName);
	
	createPageHeader(fcgi, data);
	if(userId == -1){
		fcgi->out << "<div class='errorText'>This user does not exist...</div>";
	}
	else{
		
	}
	createPageFooter(fcgi, data);
}
