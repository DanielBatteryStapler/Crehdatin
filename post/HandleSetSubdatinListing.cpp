#include "HandleSetSubdatinListing.h"

void handleSetSubdatinListing(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId == -1){
		createMustBeLoggedInErrorPage(fcgi, data);
		return;
	}
	
	std::string listNumberS;
	switch(getPostValue(fcgi->cgi, listNumberS, "listNumber", 2, InputFlag::AllowStrictOnly)){
	default:
		createSettingsPage(fcgi, data, "", "", "Invalid List Number");
		return;
	case InputError::NoError:
		break;
	}
	std::size_t listNumber;
	try{
		listNumber = std::stol(listNumberS);
	}
	catch(std::exception& e){
		createSettingsPage(fcgi, data, "", "", "Invalid List Number");
		return;
	}
	
	std::string title;
	switch(getPostValue(fcgi->cgi, title, "title", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createSettingsPage(fcgi, data, "", "", "Invalid Subdatin Title");
		return;
	case InputError::NoError:
		break;
	}
	
	size_t subdatinId = getSubdatinId(data->con, title);
	if(subdatinId == -1){
		createSettingsPage(fcgi, data, "", "", "That Subdatin Does Not Exist");
		return;
	}
	
	if(listNumber > 0){
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE subdatinListings SET listNumber = listNumber + 1 WHERE userId = ? AND listNumber >= ?"));
		prepStmt->setInt64(1, data->userId);
		prepStmt->setInt64(2, listNumber);
		prepStmt->execute();
		
		prepStmt = std::unique_ptr<sql::PreparedStatement>(data->con->prepareStatement("INSERT INTO subdatinListings (userId, subdatinId, listNumber) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE listNumber = ?"));
		prepStmt->setInt64(1, data->userId);
		prepStmt->setInt64(2, subdatinId);
		prepStmt->setInt64(3, listNumber);
		prepStmt->setInt64(4, listNumber);
		prepStmt->execute();
		
		simplifySubdatinListing(data->con, data->userId);
	}
	else{
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("DELETE FROM subdatinListings WHERE userId = ? AND subdatinId = ?"));
		prepStmt->setInt64(1, data->userId);
		prepStmt->setInt64(2, subdatinId);
		prepStmt->execute();
		
		simplifySubdatinListing(data->con, data->userId);
	}
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}

void simplifySubdatinListing(sql::Connection* con, std::size_t userId){
	std::size_t currentNum = 1;
	
	std::unique_ptr<sql::PreparedStatement> prepStmt;
	if(userId == -1){
		prepStmt.reset(con->prepareStatement("SELECT listNumber, subdatinId FROM subdatinListings WHERE userId IS NULL ORDER BY listNumber ASC"));
	}
	else{
		prepStmt.reset(con->prepareStatement("SELECT listNumber, subdatinId FROM subdatinListings WHERE userId = ? ORDER BY listNumber ASC"));
		prepStmt->setInt64(1, userId);
	}
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		if(res->getInt64("listNumber") != currentNum){
			if(userId == -1){
				std::unique_ptr<sql::PreparedStatement> prepStmtB(con->prepareStatement("UPDATE subdatinListings SET listNumber = ? WHERE userId IS NULL AND subdatinId = ?"));
				prepStmtB->setInt64(1, currentNum);
				prepStmtB->setInt64(2, res->getInt64("subdatinId"));
				prepStmtB->execute();
			}
			else{
				std::unique_ptr<sql::PreparedStatement> prepStmtB(con->prepareStatement("UPDATE subdatinListings SET listNumber = ? WHERE userId = ? AND subdatinId = ?"));
				prepStmtB->setInt64(1, currentNum);
				prepStmtB->setInt64(2, userId);
				prepStmtB->setInt64(3, res->getInt64("subdatinId"));
				prepStmtB->execute();
			}
		}
		currentNum++;
	}
}





