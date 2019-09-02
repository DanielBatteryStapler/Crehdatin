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
	safePrint("ERR: User with id '" + std::to_string(userId) + " 'has an invalid user position");
	return UserPosition::Error;
}

bool hasRainbowTextPermissions(UserPosition position){
	switch(position){
	case UserPosition::Senate:
	case UserPosition::Administrator:
	case UserPosition::Bureaucrat:
	case UserPosition::Curator:
		return true;
		break;
	}
	return false;
}

bool hasModerationPermissions(UserPosition position){
	switch(position){
	case UserPosition::Senate:
	case UserPosition::Bureaucrat:
	case UserPosition::Curator:
		return true;
		break;
	}
	return false;
}

bool hasModerationPermissionsOver(UserPosition moderator, UserPosition poster){
	int first = 0;
	switch(moderator){
	case UserPosition::Curator:
		if(poster == UserPosition::Curator){
			return false;//curators can't do anything to other curators
		}
		first = 1;
		break;
	case UserPosition::Bureaucrat:
		first = 2;
		break;
	case UserPosition::Administrator:
		switch(poster){//admins can't delete anything
		default:
			return false;
		}
		first = 3;
		break;
	case UserPosition::Senate:
		first = 4;
		break;
	default:
		return false;
		break;
	}
	
	int second = 0;
	switch(poster){
	case UserPosition::Curator:
		second = 1;
		break;
	case UserPosition::Bureaucrat:
		second = 2;
		break;
	case UserPosition::Administrator:
		second = 3;
		break;
	case UserPosition::Senate:
		second = 4;
		break;
	}
	
	return first >= second;//if they tie, let it be deleted. burs should be able to delete other burs
}

bool hasSubdatinControlPermissions(UserPosition position){
	switch(position){
	case UserPosition::Senate:
	case UserPosition::Bureaucrat:
		return true;
		break;
	}
	return false;
}

bool hasAdministrationControlPermissions(UserPosition position){
	switch(position){
	case UserPosition::Senate:
		return true;
		break;
	}
	return false;
}