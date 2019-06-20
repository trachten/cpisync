/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 8/10/2018.
//

#include "IBLTSyncTest.h"
#include "Syncs/GenSync.h"
#include "Syncs/IBLTSync.h"
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

void IBLTSyncTest::IBLTSyncReconcileTest() {
    const int BITS = sizeof(randZZ());

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setNumExpectedElements(numExpElem).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setNumExpectedElements(numExpElem).
			build();

	//(oneWay = false, probSync = true)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true));

}

void IBLTSyncTest::IBLTSyncMultiReconcileTest() {
	const int BITS = sizeof(randZZ());

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setNumExpectedElements(numExpElem).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setNumExpectedElements(numExpElem).
			build();

	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = true)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, true));
}

void IBLTSyncTest::testAddDelElem() {
    // number of elems to add
    const int ITEMS = 50;
    IBLTSync ibltSync(ITEMS, sizeof(randZZ()));
    multiset<DataObject *, cmp<DataObject*>> elts;

    // check that add works
    for(int ii = 0; ii < ITEMS; ii++) {
        DataObject* item = new DataObject(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(ibltSync.addElem(item));
    }

    // check that elements can be recovered correctly through iterators
    multiset<DataObject *, cmp<DataObject*>> resultingElts;
    for(auto iter = ibltSync.beginElements(); iter != ibltSync.endElements(); ++iter) {
        resultingElts.insert(*iter);
    }

    vector<DataObject *> diff;
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
			//Different number of expectedElements to test mismatch failure
			setNumExpectedElements(numExpElem + 100).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setNumExpectedElements(numExpElem).
			build();

	//oneWay = false, prob = true, syncParamTest = true

	CPPUNIT_ASSERT(!(syncTest(GenSyncClient, GenSyncServer, false, true, true)));
}