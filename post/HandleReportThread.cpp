#include "HandleReportThread.h"

void handleReportThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
		
		if((data->userId == -1 && data->currentTime - data->lastPostTime < Config::anonReportingTimeout()) || 
			(data->userId != -1 && data->currentTime - data->lastPostTime < Config::userReportingTimeout())){
			createGenericErrorPage(fcgi, data, "You are posting/reporting too much, wait a little longer before trying again");
			return;
		}
		
		std::string reason;
		switch(getPostValue(fcgi->cgi, reason, "reason", Config::getMaxReportLength(), InputFlag::AllowNonNormal)){
		default:
			createGenericErrorPage(fcgi, data, "Unknown Report Error");
			return;
		case InputError::IsTooLarge:
			createGenericErrorPage(fcgi, data, "Report Too Long");
			return;
		case InputError::IsEmpty:
			createGenericErrorPage(fcgi, data, "Report Cannot Be Empty");
			return;
		case InputError::ContainsNewLine:
			createGenericErrorPage(fcgi, data, "Report Cannot Contain Newlines");
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
		
		createPageHeader(fcgi, data, PageTab::None);
		fcgi->out << "Thank You For the Report! <div class='underline'><a href='" << fcgi->env->getReferrer() << "'>Back to the Thread</a></div>";
		createPageFooter(fcgi, data);
}