#include <random>
#include <string>
#include <mutex>


#include <cryptopp/osrng.h>
#include <Magick++.h>
#include <WebsiteFramework/WebsiteFramework.h>
#include <WebsiteFramework/Response.h>

#include "RequestData.h"

extern const int captchaLength;


void createCaptchaHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data);
std::string generateCaptchaText();
int64_t generateCaptchaSeed();
void createNewCaptchaSession(RequestData* data);

