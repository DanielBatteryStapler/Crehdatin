#include "HandleReportThread.h"

void handleReportThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
		
		if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonReportingTimeout()) || 
			(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userReportingTimeout())){
			handleReportThreadErrorPage(fcgi, data, "You are posting/reporting too much, wait a little longer before trying again");
			return;
		}
		
		std::string reason;
		switch(getPostValue(fcgi->cgi, reason, "reason", Config::getMaxReportLength(), InputFlag::AllowNonNormal)){
		default:
			handleReportThreadErrorPage(fcgi, data, "Unknown Report Error");
			return;
		case InputError::IsTooLarge:
			handleReportThreadErrorPage(fcgi, data, "Report Too Long");
			return;
		case InputError::IsEmpty:
			handleReportThreadErrorPage(fcgi, data, "Report Cannot Be Empty");
			return;
		case InputError::ContainsNewLine:
			handleReportThreadErrorPage(fcgi, data, "Report Cannot Contain Newlines");
			return;
		case InputError::NoError:
			break;
		}
		
		std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("INSERT INTO reports (reason, subdatinId, threadId, ip, userId) VALUES (?, ?, ?, ?, ?)"));
		prepStmtB->setString(1, reason);
		prepStmtB->setInt64(2, data->subdatinId);
		prepStmtB->setInt64(3, data->threadId);
		prepStmtB->setString(4, fcgi->env->getRemoteAddr());
		if(data->userId == -1){
			prepStmtB->setNull(5, 0);
		}
		else{
			prepStmtB->setInt64(5, data->userId);
		}
		prepStmtB->execute();
		
		setLastPostTime(fcgi, data);
		
		createPageHeader(fcgi, data);
		fcgi->out << "Thank You For the Report! <a href='https://" << WebsiteFramework::getDomain() << "/d/" << parameters[0] << "/thread/" << parameters[1] << "'>Back to the Thread</a>";
		createPageFooter(fcgi, data);
}

void handleReportThreadErrorPage(FcgiData* fcgi, RequestData* data, std::string error){
	createPageHeader(fcgi, data);
	fcgi->out << "<div class='errorText'>" << error << "</div>";
	createPageFooter(fcgi, data);
}