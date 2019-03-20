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