#include "HandleSetDefaultSubdatinListing.h"

void handleSetDefaultSubdatinListing(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createInvalidPermissionsErrorPage(fcgi, data);
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
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE subdatinListings SET listNumber = listNumber + 1 WHERE userId IS NULL AND listNumber >= ?"));
		prepStmt->setInt64(1, listNumber);
		prepStmt->execute();
		
		prepStmt.reset(data->con->prepareStatement("INSERT INTO subdatinListings (userId, isDefault, subdatinId, listNumber) VALUES (NULL, TRUE, ?, ?) ON DUPLICATE KEY UPDATE listNumber = ?"));
		prepStmt->setInt64(1, subdatinId);
		prepStmt->setInt64(2, listNumber);
		prepStmt->setInt64(3, listNumber);
		prepStmt->execute();
		
		simplifySubdatinListing(data->con, -1);
	}
	else{
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("DELETE FROM subdatinListings WHERE userId IS NULL AND subdatinId = ?"));
		prepStmt->setInt64(1, subdatinId);
		prepStmt->execute();
		
		simplifySubdatinListing(data->con, -1);
	}
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}





