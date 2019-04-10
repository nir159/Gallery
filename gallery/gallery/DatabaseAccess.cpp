#include <map>
#include <algorithm>

#include "ItemNotFoundException.h"
#include "DatabaseAccess.h"

std::list<Album> albums;// Used in the callback function
std::list<User> users;

// This function is used to run sqlite3 commands using cpp
bool DatabaseAccess::execCommand(const char* sqlStatement) {
	char **errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	if (res != SQLITE_OK)
		return false;
	return true;
}

/*
The function open the existing database or creates a new one.
return true if a database was open successfully
else return false
*/
bool DatabaseAccess::open() {
	std::string dbFileName = "Gallery.VC.db";
	int doesFileExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &db);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;
	}
	execCommand("CREATE TABLE IF NOT EXISTS USERS( ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL); ");
	execCommand("CREATE TABLE IF NOT EXISTS ALBUMS (ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, USER_ID INTEGER NOT NULL, CREATION_DATE TEXT NOT NULL, FOREIGN KEY(USER_ID) REFERENCES USERS(ID)); ");
	execCommand("CREATE TABLE IF NOT EXISTS PICTURES ( ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, LOCATION TEXT NOT NULL, CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL, FOREIGN KEY(ALBUM_ID ) REFERENCES ALBUMS(ID)); ");
	execCommand("CREATE TABLE IF NOT EXISTS TAGS ( ID INTEGER PRIMARY KEY AUTOINCREAMENT, PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, FOREIGN KEY(PICTURE_ID ) REFERENCES PICTURES(ID), FOREIGN KEY(USER_ID ) REFERENCES USERS(ID)); ");
	return true;
}

// Closes the database
void DatabaseAccess::close() {
	sqlite3_close(db);
	db = nullptr;
}

// Function clears the information from database
void DatabaseAccess::clear() {
	//clears all the allocated memory
}

// Function clears the allocated memory from openAlbum
void DatabaseAccess::closeAlbum(Album&)
{
	// Closes the album 
}

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture) {
	auto result = getAlbumIfExists(albumName);
	std::string command = "INSERT INTO PICTURES VALUES (NULL, '" + picture.getName() + "', '" + picture.getPath() + "', '" + picture.getCreationDate() + "', " + (*result).getOwnerId() + ");";
	execCommand(command.c_str());
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) {
	auto result = getAlbumIfExists(albumName);
	std::string command = "DELETE FROM PICTURES WHERE NAME = '" + pictureName + "';";
	execCommand(command.c_str());
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) {
	auto result = getPictureIfExists(pictureName);
	std::string command = "INSERT INTO TAGS VALUES (NULL, '" + (*result).getId() +"', " + userId + ");";
	execCommand(command.c_str());
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) {
	auto result = getPictureIfExists(pictureName);
	std::string command = "DELETE FROM TAGS WHERE PICTURE_ID = " + (*result).getId() + ";";
	execCommand(command.c_str());
}

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId) {

	std::string command = "DELETE FROM ALBUMS WHERE NAME = '" + albumName + ";";
	execCommand(command.c_str());
}

void DatabaseAccess::createUser(User& user) {
	std::string command = "INSERT INTO USERS VALUES (NULL, '" + user.getName() + "');";
	execCommand(command.c_str());
}

void DatabaseAccess::deleteUser(const User& user) {
	if (doesUserExists(user.getId())) {
		std::string command = "DELETE FROM USERS WHERE ID = " + std::to_string(user.getId()) + ";";
		execCommand(command.c_str());
	}
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user) {
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}

int callbackAlbums(void *data, int argc, char **argv, char **azColName)
{
	Album album;
	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "ID") {
			album.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME") {
			album.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == "USER_ID") {
			album.setOwner(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "CREATION_DATE") {
			album.setCreationDate(argv[i]);
		}
	}
	albums.push_back(album);
	return 0;
}

const std::list<Album> DatabaseAccess::getAlbums() {
	albums.clear();
	char* sqlStatement = "SELECT * FROM PERSON;";
	char *errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, callbackAlbums, nullptr, &errMessage);
	return albums;
}

const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user) {
	getAlbums();
	for (const auto& album : albums) {
		if (album.getOwnerId() != user.getId()) {
			albums.remove(album);
		}
	}
	return albums;
}

void DatabaseAccess::createAlbum(const Album& album) {
	std::string command = "INSERT INTO ALBUMS VALUES (NULL, '" + album.getName() + "', " + std::to_string(album.getOwnerId()) + ", '" + album.getCreationDate() + ");";
	execCommand(command.c_str());
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId) {
	getAlbums();
	for (const auto& album : albums) {
		if (album.getName() == albumName && album.getOwnerId() == userId) {
			return true;
		}
	}
	return false;
}

Album DatabaseAccess::openAlbum(const std::string& albumName) {
	getAlbums();
	for (const auto& album : albums) {
		if (album.getName() == albumName) {
			return album;
		}
	}
}

void DatabaseAccess::printAlbums() {
	getAlbums();
	for (const auto& album : albums) {
		std::cout << album;
	}
}

int callbackUsers(void *data, int argc, char **argv, char **azColName)
{
	User user;
	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "ID") {
			user.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME") {
			user.setName(argv[i]);
		}
	}
	users.push_back(user);
	return 0;
}

const std::list<User> DatabaseAccess::getUsers() {
	users.clear();
	char* sqlStatement = "SELECT * FROM USERS;";
	char *errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, callbackUsers, nullptr, &errMessage);
	return users;
}

void DatabaseAccess::printUsers() {
	getUsers();
	for (const auto& user : users) {
		std::cout << user;
	}
}

User DatabaseAccess::getUser(int userId) {
	getUsers();
	for (const auto& user : users) {
		if (user.getId() == userId) {
			return user;
		}
	}
	return;
}

bool DatabaseAccess::doesUserExists(int userId) {
	getUsers();
	for (const auto& user : users) {
		if (user.getId() == userId) {
			return true;
		}
	}
	return false;
}

int DatabaseAccess::countAlbumsOwnedOfUser(const User& user) {
	int ctr = 0;
	getAlbums();
	for (const auto& album : albums) {
		if (album.getOwnerId() == user.getId()) {
			ctr++;
		}
	}
	return ctr;
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user) {
	int albumsCount = 0;
	getAlbums();
	for (const auto& album : albums) {
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics) {
			if (picture.isUserTagged(user)) {
				albumsCount++;
				break;
			}
		}
	}

	return albumsCount;
}

int DatabaseAccess::countTagsOfUser(const User& user) {
	int tagsCtr = 0;
	getAlbums();
	for (const auto& album : albums) {
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics) {
			if (picture.isUserTagged(user)) {
				tagsCtr++;
			}
		}
	}

	return tagsCtr;
}

User DatabaseAccess::getTopTaggedUser() {
	getUsers();	
	User mostTagged;
	if (users.size() == NULL) {
		throw MyException("There isn't any tagged user.");
	}
	for (const auto& user : users) {
		if (countTagsOfUser(user) > countTagsOfUser(mostTagged)) {
			mostTagged = user;
		}
	}
	return mostTagged;
}

Picture DatabaseAccess::getTopTaggedPicture() {
	getAlbums();
	Picture mostTagged;
	if (users.size() == NULL) {
		throw MyException("There isn't any tagged user.");
	}
	for (const auto& album : albums) {
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics) {
			if (picture.getTagsCount() > mostTagged.getTagsCount()) {
				mostTagged = picture;
			}
		}
	}

	return mostTagged;
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user) {
	std::list<Picture> picturesTaggedOfUser;
	getAlbums();
	for (const auto& album : albums) {
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics) {
			if (picture.isUserTagged(user)) {
				picturesTaggedOfUser.push_back(picture);
			}
		}
	}

	return picturesTaggedOfUser;
}