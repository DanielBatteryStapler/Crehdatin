#pragma once

#include <string>

#include "InputHandler.h"

class MarkupOutStream;

class MarkupString{
public:
	friend class MarkupOutStream;
	friend MarkupString operator+(const MarkupString a, const MarkupString b);
	friend MarkupString operator""_m(const char* str, std::size_t len);
	friend MarkupString allowMarkup(const std::string input);
	
	MarkupString() = default;
	MarkupString(const MarkupString& input) = default;
	MarkupString(MarkupString&& input) = default;
	MarkupString(const char* input);
	MarkupString(const char* str, std::size_t len);
	MarkupString(const std::string input);
	
	std::string getUnsafeString();
	
	MarkupString& operator=(const MarkupString& input) = default;
	MarkupString& operator=(MarkupString&& input) = default;
	
	void operator+=(const MarkupString input);
	
private:
	std::string buffer;
	
	static std::string escapeMarkup(const std::string s);
};

MarkupString operator+(const MarkupString a, const MarkupString b);
MarkupString operator""_m(const char* str, std::size_t len);
MarkupString allowMarkup(const std::string input);






