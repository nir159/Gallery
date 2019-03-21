#include <map>
#include <algorithm>

#include "ItemNotFoundException.h"
#include "DatabaseAccess.h"

// This function is used to run sqlite3 commands using cpp
bool DatabaseAccess::execCommand(const char* sqlStatement) {
	char **errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	if (res != SQLITE_OK)
		return false;
	execCommand("CREATE TABLE IF NOT EXISTS ALBUMS (ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, USER_ID INTEGER NOT NULL, CREATION_DATE TEXT NOT NULL, FOREIGN KEY(USER_ID) REFERENCES USERS(ID)); ");
	execCommand("CREATE TABLE IF NOT EXISTS PICTURES ( ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, LOCATION TEXT NOT NULL, CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL, FOREIGN KEY(ALBUM_ID ) REFERENCES ALBUMS(ID)); ");
	execCommand("CREATE TABLE IF NOT EXISTS TAGS( PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, PRIMARY KEY(PICTURE_ID,USER_ID), FOREIGN KEY(PICTURE_ID ) REFERENCES PICTURES(ID), FOREIGN KEY(USER_ID ) REFERENCES USERS(ID)); ");
	execCommand("CREATE TABLE IF NOT EXISTS USERS( ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL); ");
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
	return true;
}

// Closes the database
void DatabaseAccess::close() {
	sqlite3_close(db);
	db = nullptr;
}

// Function clears the information from database
void DatabaseAccess::clear() {
	execCommand("DELETE FROM USERS; ");
	execCommand("DELETE FROM ALBUMS; ");
	execCommand("DELETE FROM PICTURES; ");
	execCommand("DELETE FROM TAGS; ");
}

// Function clears the allocated memory from openAlbum
void DatabaseAccess::closeAlbum(Album&)
{
	
}