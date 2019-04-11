#pragma once
#include "DatabaseAccess.h"

class DataAccessTest {
	DatabaseAccess dbTest;
public:
	DataAccessTest() = default;
	~DataAccessTest();
	void initDb();
	void addRows();
	void updateRows();
	void removeUser();
};