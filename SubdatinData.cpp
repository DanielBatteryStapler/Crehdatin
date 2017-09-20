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

void getSubdatinData(sql::Connection* con, int64_t id, std::string& title, std::string& name){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT title, name FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->first();
	title = res->getString("title");
	name = res->getString("name");
}

void getSubdatinLockedData(sql::Connection* con, int64_t id, bool& postLocked, bool& commentLocked){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT postLocked, commentLocked FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->first();
	postLocked = res->getBoolean("postLocked");
	commentLocked = res->getBoolean("commentLocked");
}

std::string getSubdatinTitle(sql::Connection* con, int64_t subdatinId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT title FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, subdatinId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->first();
	return res->getString("title");
}

int64_t getThreadCommentCount(sql::Connection* con, int64_t threadId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT COUNT(*) AS count FROM comments WHERE threadId = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	
	res->first();
	
	return res->getInt64("count");
}

MarkupString getFormattedTime(sql::Connection* con, int64_t time, std::string verb){
	MarkupString output;
	
	if(time < 0){
		output = verb + " Error Ago";
	}
	else if(time / 60 == 0){
		output = verb + " Less Than A Minute Ago";
	}
	else if(time / 60 == 1){
		output = verb + " A Minute Ago";
	}
	else if(time / 60 < 60){
		output = verb + " " + std::to_string(time / 60) + " Minutes Ago";
	}
	else if(time / 60 / 60 == 1){
		output = verb + " An Hour Ago";
	}
	else if(time / 60 / 60 < 24){
		output = verb + " " + std::to_string(time / 60 / 60) + " Hours Ago";
	}
	else if(time / 60 / 60 / 24 == 1){
		output = verb + " A Day Ago";
	}
	else{
		output = verb + " " + std::to_string(time / 60 / 60 / 24) + " Days Ago";
	}
	
	return output;
}

MarkupString getFormattedThreadPostTime(sql::Connection* con, int64_t threadId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT TIME_TO_SEC(TIMEDIFF(NOW(), createdTime)) AS time FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	int64_t time = res->getInt64("time");
	
	return getFormattedTime(con, time, "Posted");
}

MarkupString getFormattedThreadBumpTime(sql::Connection* con, int64_t threadId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT TIME_TO_SEC(TIMEDIFF(NOW(), lastBumpTime)) AS time FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, threadId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	int64_t time = res->getInt64("time");
	
	return getFormattedTime(con, time, "Bumped");
}

MarkupString getFormattedCommentPostTime(sql::Connection* con, int64_t commentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT TIME_TO_SEC(TIMEDIFF(NOW(), createdTime)) AS time FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, commentId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	int64_t time = res->getInt64("time");
	
	return getFormattedTime(con, time, "Posted");
}

int64_t getParentComment(sql::Connection* con, int64_t commentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT parentId FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, commentId);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->first();
	if(res->isNull("parentId")){
		return -1;
	}
	else{
		return res->getInt64("parentId");
	}
}
