#include "UserData.h"

std::string getUserName(sql::Connection* con, int64_t userId){
	std::string userName = "<removed>";
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT userName FROM users WHERE id = ?"));
	prepStmt->setInt64(1, userId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		userName = res->getString("userName");
	}
	
	return userName;
}

int64_t getUserId(sql::Connection* con, std::string userName){
	int64_t userId = -1;
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM users WHERE userName = ?"));
	prepStmt->setString(1, userName);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	if(res->next()){
		userId = res->getInt64("id");
	}
	return userId;
}

void setLastPostTime(FcgiData* fcgi, RequestData* data){
	if(!hasModerationPermissions(getEffectiveUserPosition(data->con, data->userId))){
		if(data->userId == -1){
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement(
				"INSERT INTO ips (ip, lastPostTime) VALUES (?, CURRENT_TIMESTAMP) ON DUPLICATE KEY UPDATE lastPostTime = CURRENT_TIMESTAMP"));
			prepStmt->setString(1, fcgi->env->getRemoteAddr());
			prepStmt->execute();
		}
		else{
			std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE users SET lastPostTime = CURRENT_TIMESTAMP WHERE id = ?"));
			prepStmt->setInt64(1, data->userId);
			prepStmt->execute();
		}
	}
}

MarkupString getFormattedUserString(sql::Connection* con, int64_t userId, int64_t subdatinId, bool putUserLink){
	std::string userName = getUserName(con, userId);
	UserPosition userPosition = getEffectiveUserPosition(con, userId, subdatinId);
	MarkupString output;
	
	if(putUserLink){
		output = "<a href='https://"_m + WebsiteFramework::getDomain() + "/u/" + percentEncode(userName) + "'>"_m;
	}
	
	if(userPosition == UserPosition::Senate){
		output += "<div class='senateTag'>"_m + userName + "[S]</div>"_m;
	}
	else if(userPosition == UserPosition::Administrator){
		output += "<div class='administratorTag'>"_m + userName + "[A]</div>"_m;
	}
	else if(userPosition == UserPosition::Bureaucrat){
		output += "<div class='bureaucratTag'>"_m + userName + "[B]</div>"_m;
	}
	else if(userPosition == UserPosition::Curator){
		output += "<div class='curatorTag'>"_m + userName + "[C]</div>"_m;
	}
	else if(userPosition == UserPosition::None){
		output += userName;
	}
	else{
		output += userName + "[Error]";
	}
	
	if(putUserLink){
		output += "</a>"_m;
	}
	
	return output;
}

MarkupString getFormattedPosterString(sql::Connection* con, std::string anonId, int64_t userId, int64_t subdatinId, bool putUserLink){
	if(anonId == ""){
		return "user: " + getFormattedUserString(con, userId, subdatinId, putUserLink);
	}
	else{
		return "id: " + anonId.substr(0, 8);//only show the first 8 characters to users
	}
}
