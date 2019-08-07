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

void IBLT4IBLTsSyncTest::IBLT4IBLTsSyncSetReconcileTest()
{
    const int BITS = sizeof(randZZ());
    const long numEleminSet = 5;

    GenSync GenSyncServer = GenSync::Builder()
                                .setProtocol(GenSync::SyncProtocol::IBLT4IBLTsSync)
                                .setComm(GenSync::SyncComm::socket)
                                .setBits(BITS)
                                .setNumExpectedElements(30)
                                .setNumElem(10)
                                .setSetSize(BITS)
                                //.setDataType(DATA_RECON_TYPE::SETOFSETS)
                                .build();

    GenSync GenSyncClient = GenSync::Builder()
                                .setProtocol(GenSync::SyncProtocol::IBLT4IBLTsSync)
                                .setComm(GenSync::SyncComm::socket)
                                .setBits(BITS)
                                .setNumExpectedElements(30)
                                .setNumElem(10)
                                .setSetSize(BITS)
                                //.setDataType(DATA_RECON_TYPE::SETOFSETS)
                                .build();
    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
    CPPUNIT_ASSERT(SetOfSetsSyncTest(GenSyncClient, GenSyncServer, false, false, numEleminSet));
}

void IBLT4IBLTsSyncTest::IBLTSyncLargeSetReconcileTest()
{
    const int BITS = sizeof(randZZ());

    const long numEleminSet = 5;

    GenSync GenSyncServer = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLT4IBLTsSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(largeNumExpElems).setNumElem(numEleminSet).setSetSize(BITS * numEleminSet).build();

    GenSync GenSyncClient = GenSync::Builder().setProtocol(GenSync::SyncProtocol::IBLT4IBLTsSync).setComm(GenSync::SyncComm::socket).setBits(BITS).setNumExpectedElements(largeNumExpElems).setNumElem(numEleminSet).setSetSize(BITS * numEleminSet).build();

    CPPUNIT_ASSERT(SetOfSetsSyncTest(GenSyncClient, GenSyncServer, false, true, 10));
}

void IBLT4IBLTsSyncTest::testAddDelElem()
{
    // number of elems to add
    const int ITEMS = 50;
    const int size_chldset = 5;
    IBLT4IBLTsSync iblt4IbltsSync(ITEMS, sizeof(randZZ()), size_chldset, sizeof(randZZ()));
    //multiset<DataObject *, cmp<DataObject *>> elts;
    list<DataObject *> elts;
    // check that add works
    for (int jj = 0; jj < ITEMS; jj++)
    {
        multiset<DataObject *> tmp;
        for (int ii = 0; ii < size_chldset; ii++)
        {
            DataObject *item = new DataObject(randZZ());
            tmp.insert(item);
        }
        DataObject *i = new DataObject(tmp);
        elts.push_back(i);
        CPPUNIT_ASSERT(iblt4IbltsSync.addElem(i));
    }

    list<DataObject *> resultingElts;

    for (auto iter = iblt4IbltsSync.beginElements(); iter != iblt4IbltsSync.endElements(); ++iter)
    {
        resultingElts.push_back(*iter);
    }

    CPPUNIT_ASSERT_EQUAL(elts.size(), resultingElts.size());
    if (elts.size() == resultingElts.size())
    {
        auto it2 = resultingElts.begin();
        auto it = elts.begin();

        for (int ii = 0; ii < elts.size(); ii++)
        {
            CPPUNIT_ASSERT_EQUAL((*it)->print(), (*it2)->print());
            it++;
            it2++;
        }
    }
    else
    {
        cout << "Size mismatch" << endl;
        CPPUNIT_ASSERT(false);
    }

    // check that delete works
    for (auto dop : elts)
    {
        CPPUNIT_ASSERT(iblt4IbltsSync.delElem(dop));
    }
}

void IBLT4IBLTsSyncTest::testIBLTParamMismatch()
{
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder()
                                .setProtocol(GenSync::SyncProtocol::IBLTSync)
                                .setComm(GenSync::SyncComm::socket)
                                .setBits(BITS)
                                .setNumExpectedElements(numExpElem + 100)
                                //Different number of expectedElements to ensure that mismatches cause failure properly

                                .build();

    GenSync GenSyncClient = GenSync::Builder()
                                .setProtocol(GenSync::SyncProtocol::IBLTSync)
                                .setComm(GenSync::SyncComm::socket)
                                .setBits(BITS)
                                .setNumExpectedElements(numExpElem)
                                .build();

    //(oneWay = false, probSync = true, syncParamTest = true, Multiset = false, largeSync = false)
    CPPUNIT_ASSERT(!(syncTest(GenSyncClient, GenSyncServer, false, true, true, false, false)));
}