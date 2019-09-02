#include "Response.h"

void sendStatusHeader(MarkupOutStream& fcgiOut, StatusCode code){
	fcgiOut << "Status: " << std::to_string((std::size_t)code) << "\r\n";
}

void sendCookieHeader(MarkupOutStream& fcgiOut, std::string cookieName, std::string cookieValue){
	fcgiOut << "Set-Cookie: " << cookieName << "=" << cookieValue << "; Secure; HttpOnly; Path=/; Domain=" << WebsiteFramework::getDomain() << "; Max-Age=31536000\r\n";
}

void sendDeleteCookieHeader(MarkupOutStream& fcgiOut, std::string cookieName){
	fcgiOut << "Set-Cookie: " << cookieName << "=; Secure; Path=/; SameSite=Strict; Domain=" << WebsiteFramework::getDomain() << "; Expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n";
}

void sendLocationHeader(MarkupOutStream& fcgiOut, std::string location){
	fcgiOut << "Location: " << location << "\r\n";
}

void sendContentTypeHeader(MarkupOutStream& fcgiOut, ContentType type){
	switch(type){
	case ContentType::Html:
		fcgiOut << "Content-type: text/html; charset=utf-8\r\n";
		break;
	case ContentType::GenericImage:
		fcgiOut << "Content-type: image\r\n";
		break;
	}
}

void sendContentTypeHeader(MarkupOutStream& fcgiOut, std::string mimeType){
	fcgiOut << "Content-type: " << mimeType << "\r\n";
}

void finishHttpHeader(MarkupOutStream& fcgiOut){
	fcgiOut << "\r\n";
}