#include "HandleRemoveSubdatin.h"

void handleRemoveSubdatin(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("DELETE FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, data->subdatinId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}





