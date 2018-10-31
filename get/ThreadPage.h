#pragma once

#include "../Config.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/InputHandler.h>

#include "../UserData.h"
#include "../StringHelper.h"
#include "PageHeader.h"
#include "PageFooter.h"
#include "../FormatPost.h"

enum class ThreadFlags{none = 0, isPreview = 1, showDismissReport = 2, showSubdatin = 4};
enum class CommentFlags{none = 0, isPreview = 1, showDismissReport = 2, showPoster = 4, showReplyId = 8, showSubdatin = 16, includeReplies = 32};

inline ThreadFlags operator|(ThreadFlags a, ThreadFlags b){
	return static_cast<ThreadFlags>(static_cast<std::underlying_type_t<ThreadFlags>>(a) | static_cast<std::underlying_type_t<ThreadFlags>>(b));
}
inline ThreadFlags operator&(ThreadFlags a, ThreadFlags b){
	return static_cast<ThreadFlags>(static_cast<std::underlying_type_t<ThreadFlags>>(a) & static_cast<std::underlying_type_t<ThreadFlags>>(b));
}

inline CommentFlags operator|(CommentFlags a, CommentFlags b){
	return static_cast<CommentFlags>(static_cast<std::underlying_type_t<CommentFlags>>(a) | static_cast<std::underlying_type_t<CommentFlags>>(b));
}
inline CommentFlags operator&(CommentFlags a, CommentFlags b){
	return static_cast<CommentFlags>(static_cast<std::underlying_type_t<CommentFlags>>(a) & static_cast<std::underlying_type_t<CommentFlags>>(b));
}

void createThreadPage(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
void createCommentLine(MarkupOutStream& fcgiOut, RequestData* data, bool canReply, UserPosition position, std::string& subdatinTitle, int64_t layer = 0, int64_t parentId = -1);

bool renderThread(MarkupOutStream& fcgiOut, RequestData* data, std::size_t threadId, UserPosition position, ThreadFlags flags);
//the returned bool is if you can reply to this thread or not
void renderComment(MarkupOutStream& fcgiOut, RequestData* data, std::size_t commentId, bool isEven, bool canReply, UserPosition position, CommentFlags flags);

void createReportMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, int64_t commentId = -1);
void createReplyMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t subdatinId, int64_t threadId, int64_t commentId = -1);
void createThreadModerationMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, UserPosition moderator, UserPosition poster, bool locked = false, bool stickied = false, bool showDismissReport = false);
void createCommentModerationMenu(MarkupOutStream& fcgiOut, RequestData* data, std::string& subdatinTitle, int64_t threadId, int64_t commentId, UserPosition moderator, UserPosition poster, bool showDismissReport = false);
void createPostImages(MarkupOutStream& fcgiOut, RequestData* data, int64_t threadId, int64_t commentId, bool isPreview);