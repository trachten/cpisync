//
// Created by Sean Brandenburg on 2019-06-10.
//
#include <cppunit/extensions/HelperMacros.h>
#include "Syncs/CPISync.h"
#include "CPISyncTest.h"
#include "Syncs/InterCPISync.h"
#include "TestAuxiliary.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CPISyncTest);

CPISyncTest::CPISyncTest() = default;

CPISyncTest::~CPISyncTest() = default;

void CPISyncTest::setUp() {
	const int SEED = 902;
	srand(SEED);
}

void CPISyncTest::tearDown() {}

//CPISync Test Cases

void CPISyncTest::testCPIAddDelElem() {
	// number of elems to add
	const int ITEMS = 50;
	CPISync cpisync(mBar, eltSizeSq, err,0);

	multiset<DataObject *, cmp<DataObject*>> elts;

	// check that add works
	for(int ii = 0; ii < ITEMS; ii++) {
		DataObject* item = new DataObject(randZZ());
		elts.insert(item);
		CPPUNIT_ASSERT(cpisync.addElem(item));
	}

	// check that elements can be recovered correctly through iterators
	multiset<DataObject *, cmp<DataObject*>> resultingElts;
	for(auto iter = cpisync.beginElements(); iter != cpisync.endElements(); ++iter) {
		resultingElts.insert(*iter);
	}

	vector<DataObject *> diff;
	rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
	CPPUNIT_ASSERT(diff.empty());

	// check that delete works
	for(auto dop : elts) {
		CPPUNIT_ASSERT(cpisync.delElem(dop));
	}

	CPPUNIT_ASSERT(cpisync.printElem().empty());
}

void CPISyncTest::CPISyncReconcileTest() {
		GenSync GenSyncServer = GenSync::Builder().
				setProtocol(GenSync::SyncProtocol::CPISync).
				setComm(GenSync::SyncComm::socket).
				setBits(eltSizeSq).
				setMbar(mBar).
				setErr(err).
				build();

		GenSync GenSyncClient = GenSync::Builder().
				setProtocol(GenSync::SyncProtocol::CPISync).
				setComm(GenSync::SyncComm::socket).
				setBits(eltSizeSq).
				setMbar(mBar).
				setErr(err).
				build();

		//(oneWay = false, probSync = false)
		CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false));
}

void CPISyncTest::ProbCPISyncReconcileTest() {
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::ProbCPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSizeSq).
			setMbar(mBar).
			setErr(err).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::ProbCPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSizeSq).
			setMbar(mBar).
			setErr(err).
			build();

	//(oneWay = false, probSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false));
}

//InterCPISync Test Cases

void CPISyncTest::testInterCPIAddDelElem() {
	// number of elems to add
	const int ITEMS = 50;
	InterCPISync interCPISyncServer(mBar, eltSizeSq, err, partitions);

	multiset<DataObject *, cmp<DataObject*>> elts;

	// check that add works
	for(int ii = 0; ii < ITEMS; ii++) {
		DataObject* item = new DataObject(randZZ());
		elts.insert(item);
		CPPUNIT_ASSERT(interCPISyncServer.addElem(item));
	}

	// check that elements can be recovered correctly through iterators
	multiset<DataObject *, cmp<DataObject*>> resultingElts;
	for(auto iter = interCPISyncServer.beginElements(); iter != interCPISyncServer.endElements(); ++iter) {
		resultingElts.insert(*iter);
	}

	vector<DataObject *> diff;
	rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
	CPPUNIT_ASSERT(diff.empty());

	// check that delElem works
	for(auto dop : elts) {
		CPPUNIT_ASSERT(interCPISyncServer.delElem(dop));
	}

}

void CPISyncTest::InterCPISyncReconcileTest() {
	//A small mBar so that InterCPISync is forced to recurse
	const int interCPImBar = 2;

	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::InteractiveCPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSizeSq).
			setMbar(interCPImBar).
			setNumPartitions(numParts).
			build();

	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::InteractiveCPISync).
			setComm(GenSync::SyncComm::socket).
			setBits(eltSizeSq).
			setMbar(interCPImBar).
			setNumPartitions(numParts).
			build();

	//(oneWay = false, probSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, false));
}
