#include "HandleAddSubdatin.h"

void handleHideThumbnails(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId, data->subdatinId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT fileName FROM images WHERE threadId = ? OR commentId = ?"));
	if(data->commentId == -1){
		prepStmt->setInt64(1, data->threadId);
		prepStmt->setNull(2, 0);
	}
	else{
		prepStmt->setNull(1, 0);
		prepStmt->setInt64(2, data->commentId);
	}
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		boost::filesystem::remove(Config::getCrehdatinDataDirectory() / "postImageThumbnails" / std::string(res->getString("fileName")));//remove the thumbnail file if it exists
	}
	
	prepStmt.reset(data->con->prepareStatement("UPDATE images SET showThumbnail = FALSE WHERE threadId = ? OR commentId = ?"));
	if(data->commentId == -1){
		prepStmt->setInt64(1, data->threadId);
		prepStmt->setNull(2, 0);
	}
	else{
		prepStmt->setNull(1, 0);
		prepStmt->setInt64(2, data->commentId);
	}
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
