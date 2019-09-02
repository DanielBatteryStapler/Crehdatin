#pragma once

#include "WebsiteFramework.h"

enum StatusCode{
	Ok = 200,
	SeeOther = 303,
	Unauthorized = 401,
	NotFound = 404,
	MethodNotAllowed = 405,
	UnprocessableEntity = 422
};

enum ContentType{
	Html,
	GenericImage
};

void sendStatusHeader(MarkupOutStream& fcgiOut, StatusCode code);
void sendCookieHeader(MarkupOutStream& fcgiOut, std::string cookieName, std::string cookieValue);
void sendDeleteCookieHeader(MarkupOutStream& fcgiOut, std::string cookieName);
void sendLocationHeader(MarkupOutStream& fcgiOut, std::string location);
void sendContentTypeHeader(MarkupOutStream& fcgiOut, ContentType type);
void sendContentTypeHeader(MarkupOutStream& fcgiOut, std::string mimeType);
void finishHttpHeader(MarkupOutStream& fcgiOut);

