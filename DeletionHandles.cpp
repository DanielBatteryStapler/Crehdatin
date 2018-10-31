#include "DeletionHandles.h"

void deleteSubdatin(sql::Connection* con, std::size_t id){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM threads WHERE subdatinId = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		deleteThread(con, res->getInt64("id"));
	}
	prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM subdatins WHERE id = ?"));
	prepStmt->setInt64(1, id);
	prepStmt->execute();
}

void deleteThread(sql::Connection* con, std::size_t id){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM comments WHERE threadId = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		deleteComment(con, res->getInt64("id"));
	}
	
	deleteImages(con, id, -1);
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM threads WHERE id = ?"));
	prepStmt->setInt64(1, id);
	prepStmt->execute();
}

void deleteComment(sql::Connection* con, std::size_t id){
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("SELECT id FROM comments WHERE parentId = ?"));
	prepStmt->setInt64(1, id);
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		deleteComment(con, res->getInt64("id"));
	}
	
	deleteImages(con, -1, id);
	
	prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM comments WHERE id = ?"));
	prepStmt->setInt64(1, id);
	prepStmt->execute();
}

void deleteImages(sql::Connection* con, std::size_t threadId, std::size_t commentId){
	std::unique_ptr<sql::PreparedStatement> prepStmt;
	if(threadId != -1){
		prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT id, fileName FROM images WHERE threadId = ?"));
		prepStmt->setInt64(1, threadId);
	}
	else if(commentId != -1){
		prepStmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT id, fileName FROM images WHERE commentId = ?"));
		prepStmt->setInt64(1, commentId);
	}
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	while(res->next()){
		deleteImage(con, res->getInt64("id"), res->getString("fileName"));
	}
}

void deleteImage(sql::Connection* con, std::size_t id, std::string fileName){
	//make this actually delete the image file and thumbnail file
	boost::filesystem::remove(Config::getCrehdatinDataDirectory() / "postImages" / fileName);
	boost::filesystem::remove(Config::getCrehdatinDataDirectory() / "postImageThumbnails" / fileName);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement("DELETE FROM images WHERE id = ?"));
	prepStmt->setInt64(1, id);
	prepStmt->execute();
}






