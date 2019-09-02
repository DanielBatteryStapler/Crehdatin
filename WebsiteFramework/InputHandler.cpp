#include "InputHandler.h"

namespace{
	inline bool flagContains(InputFlag a, InputFlag b){
		return static_cast<bool>(a & b);
	}
};

std::string percentDecode(const std::string& SRC){//works with utf8
	std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < SRC.length(); i++) {
        if (int(SRC[i]) == 37) {
            sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else {
            ret+=SRC[i];
        }
    }
    return ret;
}

std::string percentEncode(const std::string &value) {//works with utf8
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

//utf-8 is nice
InputError isValidText(const std::string& s, InputFlag flags){
	int next = 0;
	bool returnBefore = false;
	for(auto i = s.begin(); i != s.end(); i++){
		//if it is non single character unicode
		if(*i & 0b10000000){//is 1xxxxxx
			if(!flagContains(flags, InputFlag::AllowNonNormal)){//normal does not allow for any non single byte unicode
				return InputError::ContainsNonNormal;
			}
			if(next == 0){
				if(returnBefore){
					return InputError::IsMalformed;//return must have a newline after it
				}
				//is extra byte, but not expecting any
				if((*i & 0b11000000) == 0b10000000){//is 10xxxxxx
					return InputError::IsMalformed;
				}
					
				//1 extra byte
				if((*i & 0b11100000) == 0b11000000){//is 110xxxxx
					next = 1;
				}
				//2 extra bytes
				else if((*i & 0b11110000) == 0b11100000){//is 1110xxxx
					next = 2;
				}
				//3 extra bytes
				else if((*i & 0b11111000) == 0b11110000){//is 11110xxx
					next = 3;
				}
				//more than 4bytes is invalid
				else{
					return InputError::IsMalformed;
				}
			}
			else{
				if((*i & 0b11000000) == 0b10000000){//is 10xxxxxx
					next--;//it is good
				}
				else{//anything else is wrong
					return InputError::IsMalformed;
				}
			}
		}
		else{//is a unicode 1 byte character
			if(next != 0){//cannot have a next byte and then have a valid ascii character
				return InputError::IsMalformed;
			}
			
			if(!flagContains(InputFlag::AllowNonNormal, flags)){
				if(!isalnum(*i) && *i != '_' && *i != '-'){
					return InputError::ContainsNonNormal;
				}
			}
			
			if(!flagContains(InputFlag::AllowNewLine, flags) && (*i == '\n' || *i == '\r')){
				return InputError::ContainsNewLine;
			}
			
			if(*i <= char(31) && *i != '\n' && *i != '\r' && *i != '\t'){//make sure this doesn't mess with newline
				return InputError::IsMalformed;
			}
			
			if(*i == char(127)){
				return InputError::IsMalformed;
			}
			
			if(returnBefore){
				if(*i != '\n'){
					return InputError::IsMalformed;
				}
				else{
					returnBefore = false;
				}
			}
			else if(*i == '\r'){
				returnBefore = true;
			}
		}
	}
	return InputError::NoError;
}

InputError getPostValue(cgicc::Cgicc* cgi, std::string& output, std::string name, std::size_t maximumSize, InputFlag inputFlag){
	
	cgicc::const_form_iterator i = cgi->getElement(name);
	if(i == cgi->getElements().end()){
		if(flagContains(inputFlag, InputFlag::AllowBlank)){
			output = "";
			return InputError::NoError;
		}
		else{
			return InputError::IsEmpty;
		}
	}
	cgicc::FormEntry postData = *i;
	if(postData.length() == 0){
		if(flagContains(inputFlag, InputFlag::AllowBlank)){
			output = "";
			return InputError::NoError;
		}
		else{
			return InputError::IsEmpty;
		}
	}
	if(postData.length() > maximumSize){
		return InputError::IsTooLarge;
	}
	output = postData.getValue();
	if(!flagContains(inputFlag, InputFlag::DontCheckInputContents)){
		return isValidText(output, inputFlag);
	}
	
	return InputError::NoError;
}























