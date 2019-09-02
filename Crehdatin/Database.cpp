#include "Database.h"


bool Database::createDatabase(){
	std::cout << "You are about to create the database\n";
	std::cout << "This will create an sql database using the details provided in the config\n";
	std::cout << "This will not use the sql username and password, though\n";
	std::cout << "It will ask for you to input a user with the proper permissions to create a database\n";
	std::cout << "The sql database will be called \"" << Config::getSqlDatabaseName() << "\" change this by editting the config\n";
	std::cout << "Do you want to proceed?\n";
	
	{
		std::string input;
		do{
			std::cout << "(y/n)?";
			std::getline(std::cin, input);
		} while(input != "y" && input != "Y" && input != "n" && input != "N");
		if(input == "y" || input == "Y"){
			std::cout << "Proceeding with installation\n";
		}
		else{
			std::cout << "Cancelled\n";
			return false;
		}
	}
	
	sql::Driver* driver = nullptr;
	sql::Connection* con = nullptr;
	sql::PreparedStatement* prepStmt = nullptr;
	sql::Statement* stmt = nullptr;
	sql::ResultSet* res = nullptr;
	
	driver = sql::mysql::get_driver_instance();
	{
		std::string username;
		std::string password;
		
		std::cout << "Enter sql user details\n";
		std::cout << "User must have enough permission to create a database\n";
		std::cout << "Username: ";
		std::getline(std::cin, username);
		std::cout << "Password: ";
		std::getline(std::cin, password);
		try{
			con = driver->connect(Config::getSqlAddress(), username, password);
		}
		catch(sql::SQLException& e){
			std::cout << "Error when connecting to sql server:\"" << e.what() << "\"\nAborting...\n";
			return false;
		}
	}
	
	prepStmt = con->prepareStatement("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?");
	prepStmt->setString(1, Config::getSqlDatabaseName());
	res = prepStmt->executeQuery();
	if(res->rowsCount() > 0){
		std::cout << "Database already exists, you must uninstall before installing again\n";
		delete res;
		delete prepStmt;
		delete con;
		return false;
	}
	
	delete res;
	delete prepStmt;
	stmt = con->createStatement();
	stmt->execute("CREATE DATABASE " + Config::getSqlDatabaseName() + " DEFAULT CHARACTER SET utf8mb4");
	//this would technically allow for sql injection attacks, don't make your own database name an injection attack
 	
	con->setSchema(Config::getSqlDatabaseName());//set the database to the current database
	
	stmt->execute("SET NAMES utf8mb4");
	
	stmt->execute("CREATE TABLE users("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"userName TEXT NOT NULL,"
	"shownId TEXT DEFAULT NULL,"
	"passwordHash TEXT NOT NULL,"
	"passwordSalt TEXT NOT NULL,"
	"userPosition TEXT DEFAULT NULL,"
	"cssTheme TEXT DEFAULT NULL,"
	"createdTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
	"lastPostTime TIMESTAMP NULL"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE subdatins("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"title TEXT NOT NULL,"
	"name TEXT NOT NULL,"
	"description TEXT DEFAULT NULL,"
	"postLocked BOOL NOT NULL DEFAULT FALSE,"
	"commentLocked BOOL NOT NULL DEFAULT FALSE,"
	"createdTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE userPositions("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"userId BIGINT NOT NULL,"
	"INDEX (userId),"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE CASCADE,"
	"subdatinId BIGINT NOT NULL,"
	"INDEX (subdatinId),"
	"FOREIGN KEY (subdatinId) REFERENCES subdatins(id) ON DELETE CASCADE,"
	"userPosition TEXT NOT NULL"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE sessions("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"sessionToken TEXT(" + std::to_string(Config::getUniqueTokenLength()) + ") NOT NULL,"
	"INDEX (sessionToken(" + std::to_string(Config::getUniqueTokenLength()) + ")),"
	"captchaCode TEXT NOT NULL,"
	"captchaSeed BIGINT NOT NULL,"
	"authToken TEXT(" + std::to_string(Config::getUniqueTokenLength()) + ") NOT NULL,"
	"userId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE SET NULL,"
	"shownId TEXT DEFAULT NULL,"
	"createdTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
	"ip TEXT NOT NULL"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE threads("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"title TEXT NOT NULL,"
	"body TEXT NOT NULL,"
	"anonId TEXT DEFAULT NULL,"
	"userId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE SET NULL,"
	"stickied BOOL NOT NULL DEFAULT FALSE,"
	"locked BOOL NOT NULL DEFAULT FALSE,"
	"posterIp TEXT NOT NULL,"
	"subdatinId BIGINT NOT NULL,"
	"INDEX (subdatinId),"
	"FOREIGN KEY (subdatinId) REFERENCES subdatins(id) ON DELETE CASCADE,"
	"createdTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
	"lastBumpTime TIMESTAMP NOT NULL"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE comments("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"body TEXT NOT NULL,"
	"anonId TEXT DEFAULT NULL,"
	"userId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE SET NULL,"
	"posterIp TEXT NOT NULL,"
	"subdatinId BIGINT NOT NULL,"
	"INDEX (subdatinId),"
	"FOREIGN KEY (subdatinId) REFERENCES subdatins(id) ON DELETE CASCADE,"
	"threadId BIGINT NOT NULL,"
	"INDEX (threadId),"
	"FOREIGN KEY (threadId) REFERENCES threads(id) ON DELETE CASCADE,"
	"parentId BIGINT DEFAULT NULL,"
	"INDEX (parentId),"
	"FOREIGN KEY (parentId) REFERENCES comments(id) ON DELETE CASCADE,"
	"createdTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE images("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"shownId TEXT NOT NULL,"
	"INDEX (shownId(64)),"
	"fileName TEXT NOT NULL,"
	"magickType TEXT NOT NULL,"
	"showThumbnail BOOL NOT NULL,"
	"originalName TEXT NOT NULL,"
	"threadId BIGINT DEFAULT NULL,"
	"INDEX (threadId),"
	"FOREIGN KEY (threadId) REFERENCES threads(id) ON DELETE CASCADE,"
	"commentId BIGINT DEFAULT NULL,"
	"INDEX (commentId),"
	"FOREIGN KEY (commentId) REFERENCES comments(id) ON DELETE CASCADE"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE ips("
	"ip TEXT NOT NULL,"
	"PRIMARY KEY (ip(50)),"
	"blocked BOOL NOT NULL DEFAULT FALSE,"
	"lastPostTime TIMESTAMP NOT NULL"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE reports("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"reason TEXT NOT NULL,"
	"subdatinId BIGINT NOT NULL,"
	"INDEX (subdatinId),"
	"FOREIGN KEY (subdatinId) REFERENCES subdatins(id) ON DELETE CASCADE,"
	"threadId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (threadId) REFERENCES threads(id) ON DELETE CASCADE,"
	"commentId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (commentId) REFERENCES comments(id) ON DELETE CASCADE,"
	"ip TEXT NOT NULL,"
	"userId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE SET NULL,"
	"createdTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE subdatinListings("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"subdatinId BIGINT NOT NULL,"
	"FOREIGN KEY (subdatinId) REFERENCES subdatins(id) ON DELETE CASCADE,"
	"listNumber BIGINT NOT NULL,"
	"userId BIGINT,"
	"INDEX (userId),"
	"CONSTRAINT UNIQUE (userId, subdatinId),"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE CASCADE,"
	"isDefault BOOL DEFAULT NULL,"//is default must always either be NULL or TRUE, and it should not be checked against.
	//Rather, check if userId is NULL to find defaults subdatins
	"CONSTRAINT UNIQUE (isDefault, subdatinId)"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE watchings("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"userId BIGINT NOT NULL,"
	"INDEX (userId),"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE CASCADE,"
	"commentId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (commentId) REFERENCES comments(id) ON DELETE CASCADE,"
	"threadId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (threadId) REFERENCES threads(id) ON DELETE CASCADE"
	") ENGINE = InnoDB");
	
	stmt->execute("CREATE TABLE notifications("
	"id BIGINT NOT NULL AUTO_INCREMENT,"
	"PRIMARY KEY (id),"
	"userId BIGINT NOT NULL,"
	"INDEX (userId),"
	"FOREIGN KEY (userId) REFERENCES users(id) ON DELETE CASCADE,"
	"commentId BIGINT DEFAULT NULL,"
	"FOREIGN KEY (commentId) REFERENCES comments(id) ON DELETE CASCADE"
	") ENGINE = InnoDB");
	
	stmt->execute("GRANT ALL ON " + Config::getSqlDatabaseName() + ".* TO '" + Config::getSqlUserName() + "'@'localhost'");
	
	stmt->execute("FLUSH PRIVILEGES");
	
	delete stmt;
	delete con;
	
	//create data directory
	boost::filesystem::create_directory(Config::getCrehdatinDataDirectory());
	boost::filesystem::create_directory(Config::getCrehdatinDataDirectory() / "postImages");
	boost::filesystem::create_directory(Config::getCrehdatinDataDirectory() / "postImageThumbnails");
	
	return true;
}

bool Database::deleteDatabase(){
	std::cout << "You are about to delete the database\n";
	std::cout << "This will delete the sql database found in the config\n";
	std::cout << "This will not use the sql username and password, though\n";
	std::cout << "It will ask for you to input a user with the proper permissions to delete the database\n";
	std::cout << "The \"" << Config::getSqlDatabaseName() << "\" database will be deleted, change this by editting the config\n";
	std::cout << "THIS CANNOT BE UNDONE\n";
	std::cout << "Do you want to proceed?\n";
	
	{
		std::string input;
		do{
			std::cout << "(y/n)?";
			std::getline(std::cin, input);
		} while(input != "y" && input != "Y" && input != "n" && input != "N");
		if(input == "y" || input == "Y"){
			std::cout << "Proceeding with uninstalling\n";
		}
		else{
			std::cout << "Cancelled\n";
			return false;
		}
	}
	
	sql::Driver* driver = nullptr;
	sql::Connection* con = nullptr;
	sql::PreparedStatement* prepstmt = nullptr;
	sql::Statement* stmt = nullptr;
	sql::ResultSet* res = nullptr;

	driver = sql::mysql::get_driver_instance();
	{
		std::string username;
		std::string password;
		
		std::cout << "Enter sql user details\n";
		std::cout << "User must have enough permission to delete the database\n";
		std::cout << "Username: ";
		std::getline(std::cin, username);
		std::cout << "Password: ";
		std::getline(std::cin, password);
		try{
			con = driver->connect(Config::getSqlAddress(), username, password);
		}
		catch(sql::SQLException& e){
			std::cout << "Error when connecting to sql server:\"" << e.what() << "\"\nAborting...";
			return 0;
		}
	}
	
	prepstmt = con->prepareStatement("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?");
	prepstmt->setString(1, Config::getSqlDatabaseName());
	res = prepstmt->executeQuery();
	if(res->rowsCount() == 0){
		std::cout << "Database doesn't exist, you must install before uninstalling\n";
		delete res;
		delete prepstmt;
		delete con;
		return false;
	}
	
	delete res;
	delete prepstmt;
	
	stmt = con->createStatement();
	stmt->execute("DROP DATABASE " + Config::getSqlDatabaseName());
	//this would technically allow for sql injection attacks, don't make your database name an injection attack
	
	delete stmt;
	delete con;
	
	//delete data directory
	boost::filesystem::remove_all(Config::getCrehdatinDataDirectory());
	
	return true;
}

bool Database::checkDatabase(){
	{
		sql::Driver* driver = sql::mysql::get_driver_instance();
		std::unique_ptr<sql::Connection> con(driver->connect(Config::getSqlAddress(), Config::getSqlUserName(), Config::getSqlPassword()));
		
		std::unique_ptr<sql::PreparedStatement> prepstmt(con->prepareStatement("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?"));
		prepstmt->setString(1, Config::getSqlDatabaseName());
		std::unique_ptr<sql::ResultSet> res(prepstmt->executeQuery());
		if(res->rowsCount() == 0){
			std::cout << "Database doesn't exist, you must install Crehdatin first.\n";
			return false;
		}
	}
	
	if(!boost::filesystem::is_directory(Config::getCrehdatinDataDirectory())){
		std::cout << "Crehdatin Data Directory \"" << Config::getCrehdatinDataDirectory().string() << "\" either does not exist or is not a directory.\n";
		return false;
	}
	
	if(!boost::filesystem::is_directory(Config::getCrehdatinDataDirectory() / "postImages")){
		std::cout << "Crehdatin Data Directory \"" << Config::getCrehdatinDataDirectory().string() << "/postImages\" either does not exist or is not a directory.\n";
		return false;
	}
	
	if(!boost::filesystem::is_directory(Config::getCrehdatinDataDirectory() / "postImageThumbnails")){
		std::cout << "Crehdatin Data Directory \"" << Config::getCrehdatinDataDirectory().string() << "/postImageThumbnails\" either does not exist or is not a directory.\n";
		return false;
	}
	
	return true;
}
