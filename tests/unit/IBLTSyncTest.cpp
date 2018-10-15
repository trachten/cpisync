//
// Created by eliez on 8/10/2018.
//

#include "IBLTSyncTest.h"
#include "GenSync.h"
#include "IBLTSync.h"
#include "IBLTSync_SetDiff.h"
#include "TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(IBLTSyncTest);

IBLTSyncTest::IBLTSyncTest() = default;

IBLTSyncTest::~IBLTSyncTest() = default;

void IBLTSyncTest::setUp() {
    const int SEED = 91;
    srand(SEED);
}

void IBLTSyncTest::tearDown() {
}

void IBLTSyncTest::justSyncTest() {
    const int BITS = sizeof(randZZ());
    const int EXP_ELEM = UCHAR_MAX * 2;

    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedElements(EXP_ELEM).
            build();

    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedElements(EXP_ELEM).
            build();

    syncTestProb(GenSyncServer, GenSyncClient);

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

void IBLTSyncTest::stringReconFullTest() {
    const int BITS = 8;
    const int EXP_NumE = 500;
    const int EXP_Diff = 40;

    GenSync Alice = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedElements(EXP_NumE).
            build();

    GenSync Bob = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedElements(EXP_NumE).
            build();

    vector<ZZ> ALL_ELEM;
    for (int i = 0; i < EXP_NumE; ++i) {
        ALL_ELEM.push_back(StrtoZZ(randAsciiStr(4)));
    }

    for (int j = 0; j < EXP_NumE; ++j) {
        Alice.addElem(new DataObject(ALL_ELEM[j]));
    }

    for (int j = 0; j < EXP_Diff; ++j) {
        Bob.addElem(new DataObject(ALL_ELEM[j]));
    }
    forkHandleReport res = forkHandle(Alice,Bob, true);
    auto a = Alice.dumpElements();
cout << "Comm:" + to_string(res.bytes)<<endl;
cout << "Comm Tot:" + to_string(res.bytesTot)<<endl;
cout << "Time:" + to_string(res.totalTime)<<endl;

}