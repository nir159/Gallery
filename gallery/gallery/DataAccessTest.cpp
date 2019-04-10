#include "DataAccessTest.h"

DataAccessTest::~DataAccessTest() {
	dbTest.close();
}

void DataAccessTest::initDb() {
	dbTest = DatabaseAccess();
	dbTest.open();

}