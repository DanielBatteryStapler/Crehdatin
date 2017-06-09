#include "HandleDeleteComment.h"

void handleDeleteComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("DELETE FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, data->commentId);
	prepStmt->execute();
	
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}