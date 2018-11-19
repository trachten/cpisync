//
// Created by Bowen Song on 10/14/18.
//

#include "IBLTSyncDiffTest.h"
#include "GenSync.h"
#include "IBLTSync.h"
#include "IBLTSync_SetDiff.h"
#include "TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(IBLTSyncDiffTest);

IBLTSyncDiffTest::IBLTSyncDiffTest() = default;

IBLTSyncDiffTest::~IBLTSyncDiffTest() = default;

void IBLTSyncDiffTest::setUp() {
    const int SEED = 617;
    srand(SEED);
}

void IBLTSyncDiffTest::tearDown() {
}

void IBLTSyncDiffTest::justSyncTest() {
    const int BITS = sizeof(randZZ());
    const int EXP_ELEM = UCHAR_MAX * 2;

    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedDifference(EXP_ELEM).
            build();

    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedDifference(EXP_ELEM).
            build();

    syncTestProb(GenSyncServer, GenSyncClient);

}

void IBLTSyncDiffTest::testAddDelElem() {
    // number of elems to add
    const int ITEMS = 50;
    IBLTSync_SetDiff ibltSync(ITEMS, sizeof(randZZ()));
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

void IBLTSyncDiffTest::stringReconFullTest() {

    const int EXP_NumE = 200;
    const int EXP_Diff = 49;
    size_t str_len = 11;
    const int BITS = sizeof(ZZ)*StrtoZZ(randAsciiStr(str_len)).size();


    GenSync Alice = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedElements(EXP_Diff).
            build();

    GenSync Bob = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setNumExpectedElements(EXP_Diff).
            build();

    vector<ZZ> ALL_ELEM;
    for (int i = 0; i < EXP_NumE; ++i) {
        ALL_ELEM.push_back(StrtoZZ(randAsciiStr(str_len)));
    }

    for (int j = 0; j < EXP_NumE; ++j) {
        Alice.addElem(new DataObject(ALL_ELEM[j]));
    }

    for (int j = EXP_Diff; j < EXP_NumE; ++j) {
        Bob.addElem(new DataObject(ALL_ELEM[j]));
    }

    forkHandleReport res = forkHandle(Alice, Bob, false);
    cout << "Comm:" + to_string(res.bytes) << endl;
    cout << "Comm Tot:" + to_string(res.bytesTot) << endl;
    cout << "Time:" + to_string(res.totalTime) << endl;
    cout << "Diff:" + to_string(EXP_Diff) << endl;
    CPPUNIT_ASSERT(Bob.dumpElements().size() == Alice.dumpElements().size());

}