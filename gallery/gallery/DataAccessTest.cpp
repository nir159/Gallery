#include "DataAccessTest.h"

#define AMOUNT 3
#define END 5

int i = 0;

DataAccessTest::~DataAccessTest() {
	dbTest.close();
}

void DataAccessTest::initDb() {
	dbTest = DatabaseAccess();
	dbTest.open();
}

void DataAccessTest::addRows() {
	std::string users[AMOUNT] = { "Adam", "Nir", "Itay" };
	std::string albums[AMOUNT] = { "aAlbum", "nAlbum", "iAlbum" };
	std::string pictures[AMOUNT+AMOUNT] = { "pic1", "pic2", "pic3", "pic4", "pic5", "pic6" };
	
	for (i = 0; i < AMOUNT; i++) {
		dbTest.createUser(User(i, users[i]));// The id increases automaticlly
		dbTest.createAlbum(Album(i, albums[i]));
		dbTest.addPictureToAlbumByName(albums[i], Picture(i, pictures[i]));
		dbTest.addPictureToAlbumByName(albums[i], Picture(END-i, pictures[END-i]));
		std::list<User> us = dbTest.getUsers();
		for (const auto& u : us) {
			dbTest.tagUserInPicture(albums[i], pictures[i], u.getId());
			dbTest.tagUserInPicture(albums[i], pictures[END-i], u.getId());
		}
	}
}

void DataAccessTest::updateRows() {
	dbTest.addPictureToAlbumByName("aAlbum", Picture(i+AMOUNT, "myFemily"));
	dbTest.updateName("myFemily", "myFamily");

}