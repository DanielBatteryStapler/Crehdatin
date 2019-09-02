#pragma once

#include "StringHelper.h"
#include <stack>
#include <vector>
#include <sstream>

#include "Config.h"
#include "UserPermissions.h"
#include "SubdatinData.h"
#include "get/ThreadPage.h"

class FormatPrimitive{
public:
	virtual ~FormatPrimitive() = default;
	virtual int validStart(const std::string& buffer, std::size_t i) = 0;
	virtual std::string startTag(const std::string& buffer){throw std::invalid_argument("Can not call startTag on this format");};
	virtual int validEnd(const std::string& buffer, std::size_t i) = 0;
	virtual std::string endTag(const std::string& buffer){throw std::invalid_argument("Can not call endTag on this format");};
	virtual std::string finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies){throw std::invalid_argument("cannot call finishExclusiveFormat on this format");};
	bool isExclusive;
};

class BasicFormat: public FormatPrimitive{
public:
	BasicFormat(std::string _marker, std::string _htmlStart, std::string _htmlEnd);
	virtual int validStart(const std::string& buffer, std::size_t i);
	virtual std::string startTag(const std::string& buffer);
	virtual int validEnd(const std::string& buffer, std::size_t i);
	virtual std::string endTag(const std::string& buffer);
	std::string marker;
	std::string htmlStart;
	std::string htmlEnd;
};

class HyperLinkFormat: public FormatPrimitive{
public:
	HyperLinkFormat();
	virtual int validStart(const std::string& buffer, std::size_t i);
	virtual int validEnd(const std::string& buffer, std::size_t i);
	virtual std::string finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies);
};

class GreenTextFormat: public FormatPrimitive{
public:
	GreenTextFormat();
	virtual int validStart(const std::string& buffer, std::size_t i);
	virtual std::string startTag(const std::string& buffer);
	virtual int validEnd(const std::string& buffer, std::size_t i);
	virtual std::string endTag(const std::string& buffer);
};

class ReplyFormat: public FormatPrimitive{
public:
	ReplyFormat();
	virtual int validStart(const std::string& buffer, std::size_t i);
	virtual int validEnd(const std::string& buffer, std::size_t i);
	virtual std::string finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies);
};

class CodeFormat: public FormatPrimitive{
public:
	CodeFormat();
	virtual int validStart(const std::string& buffer, std::size_t i);
	virtual int validEnd(const std::string& buffer, std::size_t i);
	virtual std::string finishExclusiveFormat(const std::string& start, const std::string& end, const std::string& internal, RequestData* data, bool includeReplies);
};

MarkupString formatUserPostBody(RequestData* data, std::string body, bool canRainbow, bool includeReplies = true);