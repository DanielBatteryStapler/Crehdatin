#include "SubdatinAboutPage.h"

void createSubdatinAboutPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::About);
	fcgi->out << "<h1>About</h1>"
	"<ul>"
	"<div class='title'>Subdatin Officials</div>";
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT userId, userPosition FROM userPositions WHERE subdatinId = ?"));
	prepStmt->setInt64(1, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	if(res->next()){
		do{
			std::string userName = getUserName(data->con, res->getInt64("userId"));
			fcgi->out << 
			"<li><a href='https://" << WebsiteFramework::getDomain() << "/u/" << percentEncode(userName) << "'>";
			
			if(res->getString("userPosition") == "bureaucrat"){
				fcgi->out << "<div class='bureaucratTag'>" << userName << "[B]</div>";
			}
			else if(res->getString("userPosition") == "curator"){
				fcgi->out << "<div class='curatorTag'>" << userName << "[C]</div>";
			}
			else{
				fcgi->out << "<div class='errorText'>an unknown error occurred!</div>";
			}
			fcgi->out << "</a></li>";
			
		}while(res->next());
	}
	else{
		fcgi->out << "<div class='errorText'><i>There are no subdatin officials...</i></div>";
	}
	fcgi->out << "</ul>";
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("SELECT description FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, data->subdatinId);
	res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
	res->first();
	if(!res->isNull("description")){
		fcgi->out << "<h2>Infomation</h2>";
		fcgi->out << formatUserPostBody(data, res->getString("description"),  true);
	}
	
	createPageFooter(fcgi, data);
}
