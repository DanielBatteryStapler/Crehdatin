#include "PostImageThumbnail.h"

void createPostImageThumbnailHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT fileName FROM images WHERE shownId = ?"));
	prepStmt->setString(1, parameters[0]);
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(res->next()){
		sendStatusHeader(fcgi->out, StatusCode::Ok);
		sendContentTypeForImage(fcgi->out, "JPEG");
		finishHttpHeader(fcgi->out);
		
		boost::filesystem::ifstream imageFile(Config::getCrehdatinDataDirectory() / "postImageThumbnails" / std::string(res->getString("fileName")));
		
		if(imageFile){
			fcgi->out.getUnsafeOutputStream() << imageFile.rdbuf();
		}
		else{
			createGenericErrorPage(fcgi, data, "Error 404: Image does not exist");
		}
	}
	else{
		createGenericErrorPage(fcgi, data, "Error 404: Image does not exist");
	}
}
