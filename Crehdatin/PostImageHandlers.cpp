#include "PostImageHandlers.h"

PostImageError getUploadedPostImage(cgicc::Cgicc* cgi, UploadedImageData& imageData, std::string name, std::size_t maximumSize){
	cgicc::const_file_iterator i = cgi->getFile(name);
	if(i == cgi->getFiles().end()){
		return PostImageError::NoFile;
	}
	cgicc::FormFile fileForm = *i;
	if(fileForm.getDataLength() == 0){
		return PostImageError::NoFile;
	}
	
	if(fileForm.getDataLength() > maximumSize){
		return PostImageError::IsTooLarge;
	}
	imageData.originalFileName = fileForm.getFilename();
	Magick::Blob blob(fileForm.getData().c_str(), fileForm.getDataLength());
	
	imageData.image.read(blob);
	if(!imageData.image.isValid()){
		return PostImageError::InvalidType;
	}
		
	return PostImageError::NoError;
}

void saveUploadedPostImage(sql::Connection* con, UploadedImageData&& imageData, bool showThumbnail, std::size_t threadId, std::size_t commentId){
	UploadedImageData image(imageData);
	static std::atomic<unsigned long> fileCount(0);//could possibly overflow, doesn't matter
	
	auto now = std::chrono::system_clock::now();//get milliseconds since the epoch
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
	
	unsigned long currentFileCount = fileCount++;
	
	{
		std::string fileName = std::to_string(currentTime) + '-' + std::to_string(currentFileCount);
		imageData.image.strip();//remove metadata
		image.image.write((Config::getCrehdatinDataDirectory() / "postImages" / fileName).string());
		if(showThumbnail){
			image.image.thumbnail(Magick::Geometry(100, 100));//100x100 thumbnail
			image.image.magick("JPEG");
			image.image.write((Config::getCrehdatinDataDirectory() / "postImageThumbnails" / fileName).string());
		}
		
		std::unique_ptr<sql::PreparedStatement> prepStmt(con->prepareStatement(
			"INSERT INTO images (shownId, fileName, magickType, showThumbnail, originalName, threadId, commentId) VALUES (?, ?, ?, ?, ?, ?, ?)"));
		prepStmt->setString(1, generateRandomToken().substr(0, 32));
		prepStmt->setString(2, fileName);
		prepStmt->setString(3, image.image.magick());
		prepStmt->setBoolean(4, showThumbnail);
		prepStmt->setString(5, image.originalFileName);
		if(threadId != -1){
			prepStmt->setInt64(6, threadId);
		}
		else{
			prepStmt->setNull(6, 0);
		}
		if(commentId != -1){
			prepStmt->setInt64(7, commentId);
		}
		else{
			prepStmt->setNull(7, 0);
		}
		
		prepStmt->execute();
	}
	
}
