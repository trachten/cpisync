/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Zifan Wang on 8/1/2019.
//

#include "IBLT4IBLTsSyncTest.h"
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/IBLT4IBLTsSync.h>
#include "TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(IBLT4IBLTsSyncTest);

IBLT4IBLTsSyncTest::IBLT4IBLTsSyncTest() = default;

IBLT4IBLTsSyncTest::~IBLT4IBLTsSyncTest() = default;

void IBLT4IBLTsSyncTest::setUp()
{
    const int SEED = 221;
    srand(SEED);
}

void IBLT4IBLTsSyncTest::tearDown()
{
}

void IBLT4IBLTsSyncTest::testGetStrings()
{
    IBLT4IBLTsSync IBLT4IBLTsSync(0, 0, 0, 0);

    CPPUNIT_ASSERT(!IBLT4IBLTsSync.getName().empty());
}

void IBLT4IBLTsSyncTest::IBLTSyncSetReconcileTest()
{
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(numExpElem).build();

    GenSync GenSyncClient = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(numExpElem).build();

    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
    CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, false, false));
}

void IBLT4IBLTsSyncTest::IBLTSyncMultisetReconcileTest()
{
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(numExpElem).build();

    GenSync GenSyncClient = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(numExpElem).build();

    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = true, largeSync = false)
    CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, true, false));
}

void IBLT4IBLTsSyncTest::IBLTSyncLargeSetReconcileTest()
{
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(largeNumExpElems).build();

    GenSync GenSyncClient = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(largeNumExpElems).build();

    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = true)
    CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, false, true));
}

void IBLT4IBLTsSyncTest::testAddDelElem()
{
    // number of elems to add
    const int ITEMS = 50;
    IBLTSync ibltSync(ITEMS, sizeof(randZZ()));
    multiset<DataObject *, cmp<DataObject *>> elts;

    // check that add works
    for (int ii = 0; ii < ITEMS; ii++)
    {
        DataObject *item = new DataObject(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(ibltSync.addElem(item));
    }

    // check that elements can be recovered correctly through iterators
    multiset<DataObject *, cmp<DataObject *>> resultingElts;
    for (auto iter = ibltSync.beginElements(); iter != ibltSync.endElements(); ++iter)
    {
        resultingElts.insert(*iter);
    }

    vector<DataObject *> diff;
    rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
    CPPUNIT_ASSERT(diff.empty());

    // check that delete works
    for (auto dop : elts)
    {
        CPPUNIT_ASSERT(ibltSync.delElem(dop));
    }
}

void IBLT4IBLTsSyncTest::testIBLTParamMismatch()
{
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).
                            //Different number of expectedElements to ensure that mismatches cause failure properly
                            setNumExpectedElements(numExpElem + 100)
                                .build();

    GenSync GenSyncClient = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLTSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(numExpElem).build();

    //(oneWay = false, probSync = true, syncParamTest = true, Multiset = false, largeSync = false)
    CPPUNIT_ASSERT(!(syncTest(GenSyncClient, GenSyncServer, false, true, true, false, false)));
}