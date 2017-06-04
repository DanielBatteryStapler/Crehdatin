#include "DirectoryHandles.h"

bool subdatinDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	data->subdatinId = getSubdatinId(data->con, parameters[0]);
	
	if(data->subdatinId == -1){
		createPageHeader(fcgi, data, PageTab::Error);
		fcgi->out << "<div class='errorText'>This Subdatin Does Not Exist</div>";
		createPageFooter(fcgi, data);
		return false;
	}
	
	return true;
}

bool threadDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string threadId = percentDecode(parameters[1]);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM threads WHERE id = ? AND subdatinId = ?"));
	prepStmt->setString(1, threadId);
	prepStmt->setInt64(2, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(res->next()){
		data->threadId = res->getInt64("id");
	}
	else{
		createPageHeader(fcgi, data, PageTab::Error);
		fcgi->out << "<div class='errorText'>This Thread Does Not Exist</div>";
		createPageFooter(fcgi, data);
		return false;
	}
	
	return true;
}

bool commentDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::string commentId = percentDecode(parameters[2]);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT id FROM comments WHERE id = ? AND threadId = ? AND subdatinId = ?"));
	prepStmt->setString(1, commentId);
	prepStmt->setInt64(2, data->threadId);
	prepStmt->setInt64(3, data->subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(res->next()){
		data->commentId = res->getInt64("id");
	}
	else{
		createPageHeader(fcgi, data, PageTab::Error);
		fcgi->out << "<div class='errorText'>This Comment Does Not Exist</div>";
		createPageFooter(fcgi, data);
		return false;
	}
	
	return true;
}

bool userDirectoryHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	data->userPageId = getUserId(data->con, parameters[0]);
	
	if(data->userPageId == -1){
		createPageHeader(fcgi, data, PageTab::Error);
		fcgi->out << "<div class='errorText'>This User Does Not Exist</div>";
		createPageFooter(fcgi, data);
		return false;
	}
	
	return true;
}



