#include "HandleBanIp.h"

void handleBanIp(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string ip;
	switch(getPostValue(fcgi->cgi, ip, "ip", Config::getMaxNameLength(), InputFlag::AllowNonNormal)){
	default:
		createGenericErrorPage(fcgi, data, "Unknown IP Error");
		return;
	case InputError::IsTooLarge:
		createGenericErrorPage(fcgi, data, "IP Too Long");
		return;
	case InputError::IsEmpty:
		createGenericErrorPage(fcgi, data, "IP Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("INSERT INTO ips (ip, blocked, lastPostTime) VALUES (?, TRUE, CURRENT_TIME) ON DUPLICATE KEY UPDATE blocked = TRUE"));
	prepStmt->setString(1, ip);
	prepStmt->execute();
	
	prepStmt.reset(data->con->prepareStatement("DELETE FROM threads WHERE posterIp=?"));
	prepStmt->setString(1, ip);
	prepStmt->execute();
	
	prepStmt.reset(data->con->prepareStatement("DELETE FROM comments WHERE posterIp=?"));
	prepStmt->setString(1, ip);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
