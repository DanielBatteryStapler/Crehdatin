#include "MainPage.h"

void createMainPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::Main);
	
	fcgi->out << "<h1>Creh-Datin</h1>"
	"<i>An extendable Reddit and Imageboard mix written in C++! <div class='underline'><a href='https://github.com/DanielBatteryStapler/Crehdatin'>Github</a></div></i>"
	"<h2>Latest Bumped Threads</h2>";
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM threads ORDER BY lastBumpTime DESC LIMIT 20"));
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	if(!res->next()){
		fcgi->out << "<div class='errorText'><i>No More Threads...</i></div>";
	}
	else{
		do{
			renderThread(fcgi->out, data, res->getInt64("id"), UserPosition::None, ThreadFlags::isPreview | ThreadFlags::showSubdatin);
		}while(res->next());
	}
	
	createPageFooter(fcgi, data);
}
