#include "Captcha.h"

const int captchaLength = 6;

void createCaptchaHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	sendStatusHeader(fcgi->out, StatusCode::Ok);
	sendContentTypeHeader(fcgi->out, ContentType::Image);
	finishHttpHeader(fcgi->out);
	
	const int width = 200;
	const int height = 75;
	
	Magick::Image captcha(Magick::Geometry(width, height), "white");
	captcha.font("helvetica");
	captcha.fontPointsize(50);
	std::mt19937 rand;
	std::uniform_int_distribution<int32_t> verticalTextDist(0, height - 45);
	std::uniform_int_distribution<int32_t> horizontalTextDist(-5, 5);
	std::uniform_int_distribution<int32_t> rotationTextDist(-15, 15);
	std::uniform_int_distribution<int32_t> waveLengthDist(5, 12);
	
	rand.seed(data->captchaSeed);
	std::string text = data->captchaCode;
	
	for(int i = 0; i < captchaLength; i++){
		std::string character;
		character = text[i];
		Magick::Geometry geometry;
		geometry.xOff((width / captchaLength) * i + 5 + horizontalTextDist(rand));
		geometry.yOff(verticalTextDist(rand));
		geometry.width(25);
		geometry.height(50);

		captcha.annotate(character, geometry, Magick::GravityType::CenterGravity, rotationTextDist(rand));
	}
	
	captcha.wave(1, waveLengthDist(rand));
	captcha.crop(Magick::Geometry(width, height));
	captcha.addNoise(Magick::NoiseType::PoissonNoise);
	captcha.blur(20);
	captcha.sharpen(10, 75);
	captcha.adaptiveThreshold(2, 2);
	
	Magick::Blob imageData;
	captcha.write(&imageData, "PNG");
	char* dataBuffer = (char*)imageData.data();
	std::size_t dataLength = imageData.length();
	fcgi->out.write(dataBuffer, dataLength);
}

std::string generateCaptchaText(){
	static CryptoPP::AutoSeededRandomPool generator;
	static std::mutex randomLock;
	
	std::string text;
	randomLock.lock();
	for(int i = 0; i < captchaLength; i++){
		text += "ABDEFHIJKLMNPRTSUVWXY3457"[generator.GenerateByte() % 24];
	}
	randomLock.unlock();
	return text;
}

int64_t generateCaptchaSeed(){
	static CryptoPP::AutoSeededRandomPool generator;
	static std::mutex randomLock;
	
	std::unique_lock<std::mutex> lock(randomLock);
	return ((int64_t)generator.GenerateByte()) | (((int64_t)generator.GenerateByte()) << ((int64_t)8));
}

void createNewCaptchaSession(RequestData* data){
	data->captchaCode = generateCaptchaText();
	data->captchaSeed = generateCaptchaSeed();
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("UPDATE sessions SET captchaCode = ?, captchaSeed = ? WHERE sessionToken = ?"));
	prepStmt->setString(1, data->captchaCode);
	prepStmt->setInt64(2, data->captchaSeed);
	prepStmt->setString(3, data->sessionToken);
	prepStmt->execute();
}








