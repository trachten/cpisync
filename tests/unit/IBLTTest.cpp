/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 7/17/2018.
//

#include <climits>
#include "IBLTTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IBLTTest);

IBLTTest::IBLTTest() {
}

IBLTTest::~IBLTTest() {
}

void IBLTTest::setUp() {
    const int SEED = 617;
    srand(SEED);
}

void IBLTTest::tearDown() {
}

void IBLTTest::testAll() {
    vector<pair<ZZ, ZZ>> items;
    const int SIZE = 50; // should be even
    const size_t ITEM_SIZE = sizeof(randZZ());
    for(int ii = 0; ii < SIZE; ii++) {
        items.push_back({randZZ(), randZZ()});
    }

    IBLT iblt(SIZE, ITEM_SIZE);
    for(int ii=0; ii < SIZE/2; ii++) {
        iblt.insert(items.at(ii).first, items.at(ii).second);
    }

    for(int ii=SIZE/2; ii < SIZE; ii++) {
        iblt.erase(items.at(ii).first, items.at(ii).second);
    }

    for(int ii=0; ii < SIZE; ii++) {
        IBLT ibltCopy(iblt); // make a copy each time because getting is destructive
        auto pair = items.at(ii);
        ZZ value;
        CPPUNIT_ASSERT(ibltCopy.get(pair.first, value));
        CPPUNIT_ASSERT_EQUAL(pair.second, value);
    }

    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(iblt.listEntries(plus, minus));
    CPPUNIT_ASSERT_EQUAL(items.size(), plus.size() + minus.size());
}

void IBLTTest::SerializeTest()
{
    vector<pair<ZZ, ZZ>> pos, neg, ref, neg1, pos1;
    const int SIZE = 10;
    const size_t ITEM_SIZE = sizeof(ZZ(0));
    IBLT iblt(SIZE, ITEM_SIZE);
    for (int ii = 1; ii < SIZE; ii++)
    {
        iblt.insert(ZZ(ii), ZZ(ii));
        ref.push_back({ZZ(ii), ZZ(ii)});
    }
    string str = iblt.toString();
    IBLT b(SIZE, ITEM_SIZE);
    b.reBuild(str);
    // Make sure everything's same between original IBLT and reconstructed IBLT
    CPPUNIT_ASSERT_EQUAL(str, b.toString());
    // Make sure basic functions can still be applied to reconstructed one
    CPPUNIT_ASSERT(b.listEntries(pos, neg));
}

void IBLTTest::IBLTNestedInsertRetrieveTest()
{
    multiset<shared_ptr<DataObject>> result;
    std::set<ZZ> setZZ;
    const int expEntries = 20;
    const int BYTE = 8;

    IBLT InsideIBLT(expEntries, BYTE);

    //Add data to interior IBLT
    for (int ii = 0; ii < expEntries; ii++)
    {
        int before = setZZ.size();
        ZZ data = randZZ();
        setZZ.insert(data);
        while (before == setZZ.size())
        {
            data = randZZ();
            setZZ.insert(data);
        }

        shared_ptr<DataObject> InsideData = make_shared<DataObject>(data);
        result.insert(InsideData);
        InsideIBLT.insert(InsideData->to_ZZ(), InsideData->to_ZZ());
    }


    IBLT OutsideIBLT(expEntries, BYTE);

    //Insert the inside IBLT into the outside IBLT
    OutsideIBLT.insertIBLT(result, BYTE, expEntries);

    vector<pair<ZZ, ZZ>> pos, neg;
    OutsideIBLT.listEntries(pos, neg);

    //Make sure that the inside IBLT is the same as the decoded inside IBLT
    CPPUNIT_ASSERT_EQUAL(InsideIBLT.toString(), zzToString(pos[0].first));
}