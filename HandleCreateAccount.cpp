#include "HandleCreateAccount.h"


void createCreateAccountPageHandle(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/login");
	finishHttpHeader(fcgi->out);
}


void handleCreateAccount(FcgiData* fcgi, std::vector<std::string> parameters, void* _data){
	RequestData* data = (RequestData*)_data;
	
	if(data->userId != -1){
		createGenericErrorPage(fcgi, data, "You Are Already Logged In");
		return;
	}
	
	std::string userName;
	switch(getPostValue(fcgi->cgi, userName, "userName", Config::getMaxNameLength(), InputFlag::AllowStrictOnly)){
	default:
		createLoginPage(fcgi, data, "", "Unknown Username Error");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "", "Username Too Long");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "", "Username Cannot Be Empty");
		return;
	case InputError::ContainsNonNormal: case InputError::ContainsNewLine:
		createLoginPage(fcgi, data, "", "Username Can Only Contain Letters, Numbers, Hyphen, and Underscore");
		return;
	case InputError::NoError:
		break;
	}
	
	if(userName.size() < Config::getMinUserNameLength()){
		createLoginPage(fcgi, data, "", "Username Cannot Be Shorter Than " + std::to_string(Config::getMinUserNameLength()) + " Characters");
		return;
	}
	
	std::string password;
	switch(getPostValue(fcgi->cgi, password, "password", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createLoginPage(fcgi, data, "", "Unknown Password Error");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "", "Password Too Long");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "", "Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	std::string repeatPassword;
	switch(getPostValue(fcgi->cgi, repeatPassword, "repeatPassword", Config::getMaxPasswordLength(), InputFlag::DontCheckInputContents)){
	default:
		createLoginPage(fcgi, data, "", "Unknown Repeated Password Error");
		return;
	case InputError::IsTooLarge:
		createLoginPage(fcgi, data, "", "Repeated Password Too Long");
		return;
	case InputError::IsEmpty:
		createLoginPage(fcgi, data, "", "Repeated Password Cannot Be Empty");
		return;
	case InputError::NoError:
		break;
	}
	
	if(password != repeatPassword){
		createLoginPage(fcgi, data, "", "Passwords Do Not Match");
		return;
	}
	
	std::string captcha;
	switch(getPostValue(fcgi->cgi, captcha, "captcha", captchaLength, InputFlag::AllowStrictOnly)){
	default:
		createLoginPage(fcgi, data, "", "Incorrect Captcha");
		return;
	case InputError::NoError:
		break;
	}
	std::transform(captcha.begin(), captcha.end(), captcha.begin(), ::toupper);
	if(data->captchaCode != captcha){
		createLoginPage(fcgi, data, "", "Incorrect Captcha");
		createNewCaptchaSession(data);//regenerate captcha if they got it wrong
		return;
	}
	createNewCaptchaSession(data);//regenerate captcha if they got it right
	
	std::string salt = generateRandomToken();
	std::string hash = generateSecureHash(password, salt);
	
	std::unique_ptr<sql::PreparedStatement> prepStmt(data->con->prepareStatement("INSERT INTO users "
				"(userName, passwordHash, passwordSalt) SELECT ?, ?, ? FROM DUAL WHERE NOT EXISTS "
				"(SELECT id FROM users WHERE userName=?)"));
	prepStmt->setString(1, userName);
	prepStmt->setString(2, hash);
	prepStmt->setString(3, salt);
	prepStmt->setString(4, userName);
	prepStmt->execute();
	
	std::unique_ptr<sql::ResultSet> res(data->stmt->executeQuery("SELECT ROW_COUNT()"));
	
	res->first();
	
	int64_t rowCount = res->getInt64(1);
	
	if(rowCount == 0){
		createLoginPage(fcgi, data, "", "User With That Name Already Exists");
		return;
	}
	
	res.reset(data->stmt->executeQuery("SELECT LAST_INSERT_ID()"));
	
	res->first();
	
	int64_t userId = res->getInt64(1);
	
	prepStmt.reset(data->con->prepareStatement("UPDATE sessions SET userId=?, shownId=NULL WHERE sessionToken=?"));
	prepStmt->setInt64(1, userId);
	prepStmt->setString(2, data->sessionToken);
	prepStmt->execute();
	
	//copy over subdatin listings from the default into the new user
	prepStmt.reset(data->con->prepareStatement("INSERT INTO subdatinListings (subdatinId, listNumber, userId) "
		"SELECT subdatinId, listNumber, ? AS userId FROM subdatinListings WHERE userId IS NULL"));
	prepStmt->setInt64(1, userId);
	prepStmt->execute();
	
	sendStatusHeader(fcgi->out, StatusCode::SeeOther);
	sendLocationHeader(fcgi->out, "https://" + WebsiteFramework::getDomain() + "/");
	finishHttpHeader(fcgi->out);
}








