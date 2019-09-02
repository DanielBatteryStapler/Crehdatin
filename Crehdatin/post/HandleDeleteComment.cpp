#include "HandleDeleteComment.h"

void handleDeleteComment(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasModerationPermissionsOver(getEffectiveUserPosition(data->con, data->userId, data->subdatinId), getCommentPosterPosition(data->con, data->commentId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	deleteComment(data->con, data->commentId);
	
	if(fcgi->env->getReferrer() == "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1] + "/comments/" + parameters[2]){
		sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/d/" + parameters[0] + "/thread/" + parameters[1]);
	}
	else{
		sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	}
	finishHttpHeader(fcgi->out);
}