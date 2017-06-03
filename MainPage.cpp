#include "MainPage.h"

void createMainPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	createPageHeader(fcgi, data, PageTab::Main);
	
	/*
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT title, name, postLocked, commentLocked FROM subdatins"));
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->beforeFirst();
	
	if(!res->next()){
		fcgi->out << "<div class='errorText'><i>There don't appear to be any subdatins...</i></div>";
	}
	else{
		do{
			std::string title = res->getString("title");
			std::string name = res->getString("name");
			fcgi->out << "<div class='thread'><a href='https://" << WebsiteFramework::getDomain() << "/d/" << percentEncode(title) << "'><div class='threadTitle'>"
			<< escapeHtml(name) << "</div></a><div class='extraPostInfo'><div class='postInfoElement'>/" << escapeHtml(title) << "/</div>";
			if(res->getBoolean("postLocked")){
				fcgi->out << "<div class='postInfoElement'>Posts Locked</div>";
			}
			if(res->getBoolean("commentLocked")){
				fcgi->out << "<div class='postInfoElement'>Comments Locked</div>";
			}
			fcgi->out << "</div></div>";
		}while(res->next());
	}
	*/
	fcgi->out << "<h1>Creh-Datin</h1>"
	"<h2>What is this?</h2>"
	"<p>This is a website dedicated to revealing the truth about how black people from Africa were the real Kangs, not some white boys from Europe.</p>"
	"<p>Also, this page will probably just be a new thread listing in the future.</p>";
	
	createPageFooter(fcgi, data);
}
