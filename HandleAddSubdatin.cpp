#include "HandleAddSubdatin.h"

void handleAddSubdatin(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(!hasAdministrationControlPermissions(getEffectiveUserPosition(data->con, data->userId))){
		createInvalidPermissionsErrorPage(fcgi, data);
		return;
	}
	
	std::string title;
	switch(getPostValue(fcgi->cgi, title, "title", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createGenericErrorPage(fcgi, data, "Unknown Title Error");
		return;
	case InputError::IsTooLarge:
		createGenericErrorPage(fcgi, data, "Title Too Long");
		return;
	case InputError::IsEmpty:
		createGenericErrorPage(fcgi, data, "Title Cannot Be Empty");
		return;
	case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
		createGenericErrorPage(fcgi, data, "Title Can Only Contain Letters, Numbers, Hyphen, and Underscore");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string name;
	switch(getPostValue(fcgi->cgi, name, "name", Config::getMaxNameLength(), InputFlag::AllowNonNormal)){
	default:
		createGenericErrorPage(fcgi, data, "Unknown Name Error");
		return;
	case InputError::IsTooLarge:
		createGenericErrorPage(fcgi, data, "Name Too Long");
		return;
	case InputError::IsEmpty:
		createGenericErrorPage(fcgi, data, "Name Cannot Be Empty");
		return;
	case InputError::ContainsNewLine:
		createGenericErrorPage(fcgi, data, "Name Cannot Contain Newline");
		return;
	case InputError::NoError:
		break;
	}
	
	std::unique_ptr<sql::PreparedStatement>prepStmt(data->con->prepareStatement("INSERT INTO subdatins "
				"(title, name) SELECT ?, ? FROM DUAL WHERE NOT EXISTS "
				"(SELECT id FROM subdatins WHERE title=? OR name=?)"));
	prepStmt->setString(1, title);
	prepStmt->setString(2, name);
	prepStmt->setString(3, title);
	prepStmt->setString(4, name);
	prepStmt->execute();
	
	std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT ROW_COUNT()"));
	
	res->first();
	
	int64_t rowCount = res->getInt64(1);
	
	if(rowCount == 0){
		createGenericErrorPage(fcgi, data, "A Subdatin With That Name Or Title Already Exists");
		return;
	}
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, fcgi->env->getReferrer());
	finishHttpHeader(fcgi->out);
}
