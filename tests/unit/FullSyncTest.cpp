/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.cpp
 * Author: Eliezer Pearl
 * 
 * Created on June 27, 2018, 9:49 AM
 */

#include <climits>
#include "FullSyncTest.h"
#include "Syncs/FullSync.h"
#include "TestAuxiliary.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FullSyncTest);

FullSyncTest::FullSyncTest() = default;

FullSyncTest::~FullSyncTest() = default;

void FullSyncTest::setUp() {
    const int SEED = 941;
    srand(SEED);
}

void FullSyncTest::tearDown() {
}

void FullSyncTest::FullSyncReconcileTest() {

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::FullSync).
			setComm(GenSync::SyncComm::socket).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::FullSync).
			setComm(GenSync::SyncComm::socket).
			build();
	//(oneWay = false, probSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false));
}

 void FullSyncTest::FullMultiSyncReconcileTest(){
	 GenSync GenSyncServer = GenSync::Builder().
			 setProtocol(GenSync::SyncProtocol::FullSync).
			 setComm(GenSync::SyncComm::socket).
			 build();

	 GenSync GenSyncClient = GenSync::Builder().
			 setProtocol(GenSync::SyncProtocol::FullSync).
			 setComm(GenSync::SyncComm::socket).
			 build();

	 //(oneWay = false, probSync = false, paramSyncTest = false, multiset = true)
	 CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false, false, true));
}


void FullSyncTest::testAddDelElem() {
    // number of elems to add
    const int ITEMS = 50;
    FullSync fs;
    multiset<DataObject *, cmp<DataObject*>> elts;

    // check that add works
    for(int ii = 0; ii < ITEMS; ii++) {
        DataObject* item = new DataObject(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(fs.addElem(item));
    }

    // check that elements can be recovered correctly through iterators
    multiset<DataObject *, cmp<DataObject*>> resultingElts;
    for(auto iter = fs.beginElements(); iter != fs.endElements(); ++iter) {
        resultingElts.insert(*iter);
    }

    vector<DataObject *> diff;
    rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
    CPPUNIT_ASSERT(diff.empty());

    // check that delete works
    for(auto dop : elts) {
        CPPUNIT_ASSERT(fs.delElem(dop));
    }

    CPPUNIT_ASSERT_EQUAL(0l, fs.getNumElem());
}

void FullSyncTest::testGetStrings() {
    FullSync fs;

    CPPUNIT_ASSERT(fs.printElem() != string(""));
    CPPUNIT_ASSERT(fs.getName() != string(""));
}