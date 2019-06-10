//
// Created by Sean Brandenburg on 2019-06-10.
//
#include <cppunit/extensions/HelperMacros.h>
#include "Syncs/CPISync.h"
#include "CPISyncTest.h"
#include "TestAuxiliary.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CPISyncTest);

CPISyncTest::CPISyncTest() = default;

CPISyncTest::~CPISyncTest() = default;

void CPISyncTest::setUp() {
	const int SEED = 273;
	srand(SEED);
}

void CPISyncTest::tearDown() {}


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