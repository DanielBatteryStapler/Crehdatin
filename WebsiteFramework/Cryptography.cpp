#include "Cryptography.h"

const std::size_t uniqueTokenLength = 512;

std::string generateRandomToken(){
	static CryptoPP::AutoSeededRandomPool generator;
	static std::mutex randomLock;
	
	CryptoPP::SecByteBlock byteBlock(uniqueTokenLength / 2);
	randomLock.lock();
	generator.GenerateBlock(byteBlock, byteBlock.size());//make sure that this is called in a thread safe way
	randomLock.unlock();
	
	std::string randomToken;
	//std::ostringstream hex;
	CryptoPP::HexEncoder hex(new CryptoPP::StringSink(randomToken));
	
	//for (auto i = byteBlock.begin(); i != byteBlock.end(); i++){
	//    hex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(*i);
	//}
	
	hex.Put(byteBlock, byteBlock.size());
	hex.MessageEnd();
	
	return randomToken;
	
	//return hex.str();
}

std::string generateSecureHash(std::string password, std::string salt){
	uint8_t derived[uniqueTokenLength / 2];
	
	crypto_scrypt((const uint8_t*)password.c_str(), password.size(), (const uint8_t*)salt.c_str(), salt.size(), pow(2, 15), 8, 1, derived, sizeof(derived));
	
	std::string result;
	CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(result));
	
	encoder.Put(derived, sizeof(derived));
	encoder.MessageEnd();
	
	//std::ostringstream hex;
	
	//for (int i = 0; i < uniqueTokenLength / 2; i++){
	//    hex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(derived[i]);
	//}
	
	return result;
	//return hex.str();
}