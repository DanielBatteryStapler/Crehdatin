#include "SubdatinData.h"

int64_t getSubdatinId(sql::Connection* con, std::string subdatinTitle){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM subdatins WHERE title = ?"));
	prepStmt->setString(1, subdatinTitle);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	int64_t id = -1;
	res->beforeFirst();
	if(res->next()){
		id = res->getInt64("id");
	}
	return id;
}

void getSubdatinData(sql::Connection* con, int64_t id, std::string& title, std::string& name, bool& postLocked, bool& commentLocked){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT title, name, postLocked, commentLocked FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->first();
	title = res->getString("title");
	name = res->getString("name");
	postLocked = res->getBoolean("postLocked");
	commentLocked = res->getBoolean("commentLocked");
}

int64_t getThreadCommentCount(sql::Connection* con, int64_t threadId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT COUNT(*) as count FROM comments WHERE threadId = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->first();
	
	return res->getInt64("count");
}