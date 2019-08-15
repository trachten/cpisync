/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Zifan Wang on 8/1/2019.
//

#include "IBLTSetOfSetsTest.h"
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/IBLTSetOfSets.h>
#include "TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(IBLTSetOfSetsTest);

IBLTSetOfSetsTest::IBLTSetOfSetsTest() = default;

IBLTSetOfSetsTest::~IBLTSetOfSetsTest() = default;

void IBLTSetOfSetsTest::setUp()
{
    const int SEED = 221;
    srand(SEED);
}

void IBLTSetOfSetsTest::tearDown()
{
}

void IBLTSetOfSetsTest::testGetStrings()
{
    IBLTSetOfSets IBLTSetOfSets(0, 0, 0, 0);

    CPPUNIT_ASSERT(!IBLTSetOfSets.getName().empty());
}

void IBLTSetOfSetsTest::IBLTSetOfSetsSetReconcileTest()
{
    const int BITS = sizeof(randZZ());
    const long numElemChldSet = 5;

    GenSync GenSyncServer = GenSync::Builder()
                                .setProtocol(GenSync::SyncProtocol::IBLTSetOfSets)
                                .setComm(GenSync::SyncComm::socket)
                                .setBits(BITS)
                                .setNumExpectedElements(100)
                                .setNumElem(10)
                                .setChldSetSize(BITS)
                                .build();

    GenSync GenSyncClient = GenSync::Builder()
                                .setProtocol(GenSync::SyncProtocol::IBLTSetOfSets)
                                .setComm(GenSync::SyncComm::socket)
                                .setBits(BITS)
                                .setNumExpectedElements(100)
                                .setNumElem(10)
                                .setChldSetSize(BITS)
                                .build();

    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
    CPPUNIT_ASSERT(SetOfSetsSyncTest(GenSyncClient, GenSyncServer, false, false, numElemChldSet));
}


void IBLTSetOfSetsTest::testAddDelElem()
{
    // number of elems to add
    const int ITEMS = 50;
    const int size_chldset = 5;
    IBLTSetOfSets IBLTSetOfSets(ITEMS, sizeof(randZZ()), size_chldset, sizeof(randZZ()));
    list<shared_ptr<DataObject>> elts;
    // check that add works
    for (int jj = 0; jj < ITEMS; jj++)
    {
        multiset<shared_ptr<DataObject>> tmp;
        for (int ii = 0; ii < size_chldset; ii++)
        {
            shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
            tmp.insert(item);
        }
        shared_ptr<DataObject> elem = make_shared<DataObject>(tmp);
        elts.push_back(elem);
        CPPUNIT_ASSERT(IBLTSetOfSets.addElem(elem));
    }

    list<shared_ptr<DataObject>> resultingElts;

    for (auto iter = IBLTSetOfSets.beginElements(); iter != IBLTSetOfSets.endElements(); ++iter)
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
        cout << "Size doesn't match" << endl;
        CPPUNIT_ASSERT(false);
    }

    // check that delete works
    for (auto dop : elts)
    {
        CPPUNIT_ASSERT(IBLTSetOfSets.delElem(dop));
    }
}
