/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 8/10/2018.
//

#include "IBLTSyncTest.h"
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/IBLTSync.h>
#include "TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(IBLTSyncTest);

IBLTSyncTest::IBLTSyncTest() = default;

IBLTSyncTest::~IBLTSyncTest() = default;

void IBLTSyncTest::setUp() {
    const int SEED = 93;
    srand(SEED);
}

void IBLTSyncTest::tearDown() {
}

void IBLTSyncTest::IBLTSyncSetReconcileTest() {
    const int BITS = sizeof(randZZ());

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();

	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, false, false));
}

void IBLTSyncTest::IBLTSyncMultisetReconcileTest() {
	const int BITS = sizeof(randZZ());

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();

	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = true, largeSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, true, false));
}

void IBLTSyncTest::IBLTSyncLargeSetReconcileTest() {
	const int BITS = sizeof(randZZ());

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(largeNumExpElems).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(largeNumExpElems).
			build();

	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = true)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true,false,false,true));
}

void IBLTSyncTest::testAddDelElem() {
    // number of elems to add
    const int ITEMS = 50;
    IBLTSync ibltSync(ITEMS, sizeof(randZZ()));
    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> elts;

    // check that add works
    for(int ii = 0; ii < ITEMS; ii++) {
        shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(ibltSync.addElem(item));
    }

    // check that elements can be recovered correctly through iterators
    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> resultingElts;
    for(auto iter = ibltSync.beginElements(); iter != ibltSync.endElements(); ++iter) {
        resultingElts.insert(*iter);
    }

    vector<shared_ptr<DataObject>> diff;
    rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
    CPPUNIT_ASSERT(diff.empty());

    // check that delete works
    for(auto dop : elts) {
        CPPUNIT_ASSERT(ibltSync.delElem(dop));
    }
}

void IBLTSyncTest::testGetStrings() {
    IBLTSync ibltSync(0, 0);

    CPPUNIT_ASSERT(!ibltSync.getName().empty());
}

void IBLTSyncTest::testIBLTParamMismatch(){
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			//Different number of expectedElements to ensure that mismatches cause failure properly
			setExpNumElems(numExpElem + 100).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();

	//(oneWay = false, probSync = true, syncParamTest = true, Multiset = false, largeSync = false)
	CPPUNIT_ASSERT(!(syncTest(GenSyncClient, GenSyncServer, false, true, true, false, false)));
}