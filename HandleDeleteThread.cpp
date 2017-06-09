#include "HandleDeleteThread.h"

void handleDeleteThread(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
		
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmtB(data->con->prepareStatement("DELETE FROM threads WHERE id = ?"));
	prepStmtB->setInt64(1, data->threadId);
	prepStmtB->execute();
	
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}