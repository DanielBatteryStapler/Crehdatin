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
	"<p>This is a website I've been working on. It will be a mix between Reddit and a classic imageboard. The hope is to get the best of the two systems.</p>"
	"<p>That is, to get a better discussions system than an imagebaord, which always has too much chaos, but without the problem of dissenting opinions being buried, which is a major problem with reddit.</p>"
	"<p>The main purpose of making this website is just practice, I guess. I haven't really thought about it all that much.</p>"
	"<a href='https://github.com/DanielBatteryStapler/Crehdatin'>My Github</a>";
	
	createPageFooter(fcgi, data);
}
