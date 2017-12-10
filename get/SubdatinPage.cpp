#include "SubdatinPage.h"

void createSubdatinPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::ThreadList);
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM threads WHERE subdatinId = ? ORDER BY stickied DESC, lastBumpTime DESC"));
	prepStmt->setInt64(1, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	if(!res->next()){
		fcgi->out << "<div class='errorText'><i>There doesn't appear to be any posts here...</i></div>";
	}
	else{
		do{
			renderThread(fcgi->out, data, data->subdatinId, parameters[0], res->getInt64("id"), true);
		}while(res->next());
	}
	createPageFooter(fcgi, data);
}
