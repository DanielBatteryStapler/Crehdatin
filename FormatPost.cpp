#include "FormatPost.h"

BasicFormat::BasicFormat(std::string _marker, std::string _htmlStart, std::string _htmlEnd):
	marker(_marker), htmlStart(_htmlStart), htmlEnd(_htmlEnd){
	isExclusive = false;
}

int BasicFormat::validStart(const std::string& buffer, std::size_t i){
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

std::string BasicFormat::startTag(const std::string& buffer){
	return htmlStart;
}

int BasicFormat::validEnd(const std::string& buffer, std::size_t i){
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

std::string BasicFormat::endTag(const std::string& buffer){
	return htmlEnd;
}

//=======

HyperLinkFormat::HyperLinkFormat(){
	isExclusive = true;
}

int HyperLinkFormat::validStart(const std::string& buffer, std::size_t i){
	std::string marker = "https://";
	if(buffer.size() - i < marker.size()){
		//return -1;
	}
	else if(marker == buffer.substr(i, marker.size())){
		return marker.size();
	}
	marker = "http://";
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

int HyperLinkFormat::validEnd(const std::string& buffer, std::size_t i){
	if(buffer.substr(i, 1) == "\n" || buffer.substr(i, 1) == " "){
		return 1;
	}
	return -1;
}

std::string HyperLinkFormat::finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies){
	return "<a href='" + start + internal + "'>" + start + internal + "</a>" + end;
}

//=====


GreenTextFormat::GreenTextFormat(){
	isExclusive = false;
}

int GreenTextFormat::validStart(const std::string& buffer, std::size_t i){
	std::string marker = "&gt;";
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

std::string GreenTextFormat::startTag(const std::string& buffer){
	return "<div class='greenText'>&gt;";
}

int GreenTextFormat::validEnd(const std::string& buffer, std::size_t i){
	std::string marker = "\n";
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

std::string GreenTextFormat::endTag(const std::string& buffer){
	return "</div>" + buffer;
}

//====

ReplyFormat::ReplyFormat(){
	isExclusive = true;
}

int ReplyFormat::validStart(const std::string& buffer, std::size_t i){
	std::string marker = "&gt;&gt;";
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

int ReplyFormat::validEnd(const std::string& buffer, std::size_t i){
	if(buffer.substr(i, 1) == "\n" || buffer.substr(i, 1) == " "){
		return 1;
	}
	return -1;
}

std::string ReplyFormat::finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies){
	if(includeReplies){
		std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("SELECT subdatinId, threadId FROM comments WHERE id = ?"));
		prepStmt->setString(1, internal);
		std::unique_ptr<sql::ResultSet> res(prepStmt->executeQuery());
		
		res->beforeFirst();
		if(res->next()){
			std::string subdatinTitle = getSubdatinTitle(data->con, res->getInt64("subdatinId"));
			std::ostringstream commentStream;
			MarkupOutStream markupStream(commentStream);
			renderComment(markupStream, data, res->getInt64("subdatinId"), subdatinTitle, std::stoll(internal), false, true, true, true, false, false, true, false);
			std::string commentHtml = commentStream.str();
			data->htmlId++;
			return ("<div class='replyDropDown'><input type='checkbox' id='replyDropDown" + std::to_string(data->htmlId) + "'><label for='replyDropDown" + std::to_string(data->htmlId) + "' class='replyDropBtn'>" + start + internal + "</label>" + commentHtml + "</div></div>" + end);
		}
		else{
			return start + internal + end;
		}
	}
	else{
		return start + internal + end;
	}
}

//====

CodeFormat::CodeFormat(){
	isExclusive = true;
}

int CodeFormat::validStart(const std::string& buffer, std::size_t i){
	std::string marker = "``";
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

int CodeFormat::validEnd(const std::string& buffer, std::size_t i){
	std::string marker = "``";
	if(buffer.size() - i < marker.size()){
		return -1;
	}
	return (marker == buffer.substr(i, marker.size()))?marker.size():-1;
}

std::string CodeFormat::finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies){
	return "<code>" + internal + "</code>";
}

//===


MarkupString formatUserPostBody(RequestData* data, std::string body, bool canRainbow, bool includeReplies){
	body = MarkupString(body).getUnsafeString();
	replaceAll(body, "\r", "");
	
	BasicFormat bold("**", "<span style='font-weight: bold;'>", "</span>");
	BasicFormat italics("*", "<span style='font-style: italic;'>", "</span>");
	BasicFormat underline("__", "<span style='text-decoration: underline;'>", "</span>");
	BasicFormat strikeThrough("~~", "<span style='text-decoration: line-through;'>", "</span>");
	BasicFormat header("##", "<h3'>", "</h3>");
	HyperLinkFormat hyperLink;
	GreenTextFormat greenText;
	ReplyFormat reply;
	CodeFormat code;
	
	BasicFormat rainbow("%%", "<span class='rainbowText'>", "</span>");
	
	std::vector<FormatPrimitive*> unusedFormats{&bold, &italics, &underline, &strikeThrough, &header, &hyperLink, &greenText, &reply, &code};
	if(canRainbow){
		unusedFormats.push_back(&rainbow);
	}
	
	std::stack<FormatPrimitive*> formatStack;
	
	std::string output;
	std::string start;
	std::string exclusiveContents;
	for(int i = 0; i < body.size(); i++){
		if(!formatStack.empty() && formatStack.top()->isExclusive){//to handle exclusive formats
			int length = formatStack.top()->validEnd(body, i);
			if(length == -1){
				exclusiveContents += body[i];
			}
			else{
				output += formatStack.top()->finishExclusiveFormat(start, body.substr(i, length), exclusiveContents, data, includeReplies);
				unusedFormats.push_back(formatStack.top());
				formatStack.pop();
				i += length - 1;
				exclusiveContents = "";
			}
		}
		else{
			if(!formatStack.empty()){
				int length = formatStack.top()->validEnd(body, i);
				if(length != -1){
					output += formatStack.top()->endTag(body.substr(i, length));
					unusedFormats.push_back(formatStack.top());
					formatStack.pop();
					i += length - 1;
					continue;
				}
			}
			std::vector<FormatPrimitive*>::iterator current;
			int length = -1;
			for(auto y = unusedFormats.begin(); y != unusedFormats.end(); y++){
				int tempLength = (*y)->validStart(body, i);
				if(tempLength != -1 && tempLength > length){
					current = y;
					length = tempLength;
				}
			}
			if(length == -1){
				output += body[i];//add the character on if it doesn't match with anything
			}
			else{
				start = body.substr(i, length);
				if(!(*current)->isExclusive){
					output += (*current)->startTag(start);
				}
				formatStack.push(*current);
				unusedFormats.erase(current);
				i += length - 1;
			}
		}
	}
	
	if(!formatStack.empty()){
		if(formatStack.top()->isExclusive){
			output += formatStack.top()->finishExclusiveFormat(start, "", exclusiveContents, data, includeReplies);
			formatStack.pop();
		}
		while(!formatStack.empty()){
			output += formatStack.top()->endTag("");
			formatStack.pop();
		}
	}

	replaceAll(output, "\n", "<br>");
	trimString(output);
	
	return allowMarkup(output);
}

























