#include "MarkupString.h"

MarkupString::MarkupString(const char* input):
	buffer(escapeMarkup(input)){
}

MarkupString::MarkupString(const char* str, std::size_t len):
	buffer(str, len){
	buffer = escapeMarkup(buffer);
}

MarkupString::MarkupString(const std::string input):
	buffer(escapeMarkup(input)){
}

std::string MarkupString::getUnsafeString(){
	return buffer;
}

void MarkupString::operator+=(const MarkupString input){
	buffer += input.buffer;
}

std::string MarkupString::escapeMarkup(const std::string s){
	std::stringstream output;
	for(auto i = s.begin(); i != s.end(); i++){
		if(*i == '<'){
			output << "&lt;";
		}
		else if(*i == '>'){
			output << "&gt;";
		}
		else if(*i == '&'){
			output << "&amp;";
		}
		else if(*i == ';'){
			output << "&#59;";
		}
		else if(*i == '"'){
			output << "&quot;";
		}
		else if(*i == '\''){
			output << "&#39;";
		}
		else{
			output << *i;
		}
	}
	return output.str();
}

MarkupString operator+(const MarkupString a, const MarkupString b){
	return allowMarkup(a.buffer + b.buffer);
}

MarkupString operator""_m(const char* str, std::size_t len){
	MarkupString mStr;
	mStr.buffer = std::string(str, len);//directly write into the butter without escaping
	return mStr;
}

MarkupString allowMarkup(const std::string input){
	MarkupString str;
	str.buffer = input;//directly write into the butter without escaping
	return str;
}





