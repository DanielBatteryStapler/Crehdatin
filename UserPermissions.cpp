#include "UserPermissions.h"

UserPosition getEffectiveUserPosition(sql::Connection* con, int64_t userId, int64_t subdatinId){
	std::string position;
	
	if(userId != -1){
		std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT userPosition FROM users WHERE id = ?"));
		prepStmt->setInt64(1, userId);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		res->beforeFirst();
		if(res->next()){
			if(!res->isNull("userPosition")){
				position = res->getString("userPosition");
			}
		}
		
		if(position.size() == 0 && subdatinId != -1){
			prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT userPosition FROM userPositions WHERE userId = ? AND subdatinId = ?"));
			prepStmt->setInt64(1, userId);
			prepStmt->setInt64(2, subdatinId);
			res = std::unique_ptr<sql::ResultSet>(prepStmt->executeQuery());
			res->beforeFirst();
			if(res->next()){
				position = res->getString("userPosition");
			}
		}
	}
	
	if(position == ""){
		return UserPosition::None;
	}
	else if(position == "curator"){
		return UserPosition::Curator;
	}
	else if(position == "bureaucrat"){
		return UserPosition::Bureaucrat;
	}
	else if(position == "administrator"){
		return UserPosition::Administrator;
	}
	else if(position == "senate"){
		return UserPosition::Senate;
	}
	return UserPosition::Error;
}

bool hasRainbowTextPermissions(UserPosition position){
	if(position == UserPosition::Senate || position == UserPosition::Administrator || position == UserPosition::Bureaucrat || position == UserPosition::Curator){
		return true;
	}
	return false;
}

bool hasModerationPermissions(UserPosition position){
	if(position == UserPosition::Senate || position == UserPosition::Administrator || position == UserPosition::Bureaucrat || position == UserPosition::Curator){
		return true;
	}
	return false;
}

bool hasSubdatinControlPermissions(UserPosition position){
	if(position == UserPosition::Senate || position == UserPosition::Bureaucrat){
		return true;
	}
	return false;
}

bool hasAdministrationControlPermissions(UserPosition position){
	if(position == UserPosition::Senate){
		return true;
	}
	return false;
}