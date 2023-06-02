/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.cpp
 * Author: Eliezer Pearl
 * 
 * Created on June 27, 2018, 9:49 AM
 */

#include "FullSyncTest.h"
#include <CPISync/Syncs/FullSync.h>
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

void FullSyncTest::FullSyncSetReconcileTest() {

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::FullSync).
			setComm(GenSync::SyncComm::socket).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::FullSync).
			setComm(GenSync::SyncComm::socket).
			build();
	//(oneWay = false, Multiset = false, largeSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false, false));
}

 void FullSyncTest::FullSyncMultisetReconcileTest(){
	 GenSync GenSyncServer = GenSync::Builder().
			 setProtocol(GenSync::SyncProtocol::FullSync).
			 setComm(GenSync::SyncComm::socket).
			 build();

	 GenSync GenSyncClient = GenSync::Builder().
			 setProtocol(GenSync::SyncProtocol::FullSync).
			 setComm(GenSync::SyncComm::socket).
			 build();

	 //(oneWay = false, Multiset = true, largeSync = false)
	 CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false));
}

void FullSyncTest::FullSyncLargeSetReconcileTest() {

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::FullSync).
			setComm(GenSync::SyncComm::socket).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::FullSync).
			setComm(GenSync::SyncComm::socket).
			build();

	//(oneWay = false, Multiset = false, largeSync = true)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false, true));
}


void FullSyncTest::testAddDelElem() {
    // number of elems to add
    const int ITEMS = 50;
    FullSync fs;
    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> elts;

    // check that add works
    for(int ii = 0; ii < ITEMS; ii++) {
        shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(fs.addElem(item));
    }

    // check that elements can be recovered correctly through iterators
    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> resultingElts;
    for(auto iter = fs.beginElements(); iter != fs.endElements(); ++iter) {
        resultingElts.insert(*iter);
    }

    vector<shared_ptr<DataObject>> diff;
    rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
    CPPUNIT_ASSERT(diff.empty());

    // check that delete works
    for(const auto& dop : elts) {
        CPPUNIT_ASSERT(fs.delElem(dop));
    }

    CPPUNIT_ASSERT_EQUAL(0l, fs.getNumElem());
}

void FullSyncTest::testGetStrings() {
    FullSync fs;

    CPPUNIT_ASSERT(fs.printElem() != string(""));
    CPPUNIT_ASSERT(fs.getName() != string(""));
}