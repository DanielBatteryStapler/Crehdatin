#include "PostImage.h"

void createPostImageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT fileName, magickType FROM images WHERE shownId = ?"));
	prepStmt->setString(1, parameters[0]);
	
	std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
	res->beforeFirst();
	
	if(res->next()){
		sendStatusHeader(fcgi->out, StatusCode::Ok);
		sendContentTypeForImage(fcgi->out, res->getString("magickType"));
		finishHttpHeader(fcgi->out);
		
		boost::filesystem::ifstream imageFile(Config::getCrehdatinDataDirectory() / "postImages" / std::string(res->getString("fileName")));
		
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

void sendContentTypeForImage(MarkupOutStream& fcgiOut, std::string magickType){
	if(magickType == "JPEG"){
		sendContentTypeHeader(fcgiOut, "image/jpeg");
	}
	else if(magickType == "PNG"){
		sendContentTypeHeader(fcgiOut, "image/png");
	}
	else if(magickType == "GIF"){
		sendContentTypeHeader(fcgiOut, "image/gif");
	}
	else if(magickType == "BMP"){
		sendContentTypeHeader(fcgiOut, "image/bmp");
	}
	else if(magickType == "SVG"){
		sendContentTypeHeader(fcgiOut, "image/svg+xml");
	}
	else{
		sendContentTypeHeader(fcgiOut, ContentType::GenericImage);//if we don't know what the image is, just send a generic mime type and let the browser deal with it
	}
}