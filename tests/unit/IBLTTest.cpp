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

void IBLTTest::tearDown() {}

void IBLTTest::testAll() {
    vector<pair<ZZ, ZZ>> items;
    const int SIZE = 50; // should be even
    const size_t ITEM_SIZE = sizeof(randZZ());

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back({randZZ(), randZZ()});

    IBLT iblt(SIZE * 2, ITEM_SIZE);
    for(unsigned int ii=0; ii < SIZE/2; ii++)
        iblt.insert(items.at(ii).first, items.at(ii).second);

    for(unsigned int ii=SIZE/2; ii < SIZE; ii++)
        iblt.erase(items.at(ii).first, items.at(ii).second);

    for(unsigned int ii=0; ii < SIZE; ii++) {
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
    IBLT reconstructedIBLT(SIZE, ITEM_SIZE);
    reconstructedIBLT.reBuild(str);
    // Make sure everything's same between original IBLT and reconstructed IBLT
    CPPUNIT_ASSERT_EQUAL(str, reconstructedIBLT.toString());
    // Make sure basic functions can still be applied to reconstructed one
    CPPUNIT_ASSERT(reconstructedIBLT.listEntries(pos, neg));
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
        unsigned long before = setZZ.size();
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
    OutsideIBLT.insert(result, BYTE, expEntries);

    vector<pair<ZZ, ZZ>> pos, neg;
    OutsideIBLT.listEntries(pos, neg);

    //Make sure that the inside IBLT is the same as the decoded inside IBLT
    CPPUNIT_ASSERT_EQUAL(InsideIBLT.toString(), zzToString(pos[0].first));
}

void IBLTTest::testIBLTMultisetInsert() {
    vector<pair<ZZ, ZZ>> items;
    const int SIZE = 500;
    const size_t ITEM_SIZE = sizeof(randZZ());

    // add randomly repeated entries
    for(int ii = 0; ii < SIZE; ) {
        int jj = 0, repeat = rand()%10 + 1;
        while(jj < repeat && ii < SIZE ) {
            ZZ temp = randZZ();
            items.push_back({temp, temp});
            ii++; jj++;
        }
    }

    IBLTMultiset iblt(SIZE, ITEM_SIZE);
    for(unsigned int ii=0; ii < SIZE/2; ii++) {
        iblt.insert(items.at(ii).first, items.at(ii).second);
    }

    for(unsigned int ii=SIZE/2; ii < SIZE; ii++) {
        iblt.erase(items.at(ii).first, items.at(ii).second);
    }

    for(unsigned int ii=0; ii < SIZE; ii++) {
        IBLTMultiset ibltCopy(iblt); // make a copy each time because getting is destructive
        auto pair = items.at(ii);
        ZZ value;
        bool isGet = ibltCopy.get(pair.first, value);
        CPPUNIT_ASSERT(isGet);
        CPPUNIT_ASSERT_EQUAL(pair.second, value);
    }

    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(iblt.listEntries(plus, minus));
    CPPUNIT_ASSERT_EQUAL(items.size(), plus.size() + minus.size());
}

char toChar(ZZ inp) {
    int inpVal;
    NTL::conv(inpVal, inp);
    char res = (char)inpVal ;
    return res;
}

void IBLTTest::testIBLTMultisetSubtract() {
    vector<pair<ZZ, ZZ>> items;
    multiset<ZZ> allItems;
    const int SIZE = 100;
    const size_t ITEM_SIZE = sizeof(randZZ());

    IBLTMultiset iblt(SIZE, ITEM_SIZE);

    // add randomly repeated entries
    for(int ii = 0; ii < SIZE/2; ) {
        int jj = 0, repeat = rand()%10 + 1;
        while(jj < repeat && ii < SIZE/2 ) {
            ZZ temp = randZZ();
            items.push_back({temp, temp});
//            cout << "Inserting: " << temp << endl;
            iblt.insert(items.at(ii).first, items.at(ii).second);
            allItems.insert(temp);
            ii++; jj++;
        }
    }

    IBLTMultiset iblt2(SIZE, ITEM_SIZE);

    // add randomly repeated entries
    for(int ii = SIZE/2; ii < SIZE; ) {
        int jj = 0, repeat = rand()%10 + 1;
        while(jj < repeat && ii < SIZE ) {
            ZZ temp = randZZ();
            items.push_back({temp, temp});
//            cout << "Inserting: " << temp << endl;
            iblt2.insert(items.at(ii).first, items.at(ii).second);
            allItems.insert(temp);
            ii++; jj++;
        }
    }


    IBLTMultiset iblt2Copy(iblt2);
    iblt2Copy -= iblt;
    iblt -= iblt2;

    for(unsigned int ii=0; ii < SIZE; ii++) {
        IBLTMultiset ibltCopy(iblt); // make a copy each time because getting is destructive
        auto pair = items.at(ii);
        ZZ value;
        bool isGet = ibltCopy.get(pair.first, value);
        long k, v, s;
        NTL::conv(k, pair.first);
        NTL::conv(v, value);
        NTL::conv(s, pair.second);
        CPPUNIT_ASSERT(isGet);
        CPPUNIT_ASSERT_EQUAL(pair.second, value);
    }

    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(iblt.listEntries(plus, minus));
    multiset<ZZ> pos, neg, recon;
    for (auto elem: plus) {
        pos.insert(elem.first);
    }
    for (auto elem: minus) {
        neg.insert(elem.first);
    }
    recon = multisetUnion(pos, neg);

    CPPUNIT_ASSERT_EQUAL(items.size(), plus.size() + minus.size());
    CPPUNIT_ASSERT(recon == allItems);

    pos.clear(); neg.clear();
    plus.clear(); minus.clear();
    CPPUNIT_ASSERT(iblt2Copy.listEntries(plus, minus));
    for (auto elem: plus) pos.insert(elem.first);
    for (auto elem: minus) neg.insert(elem.first);
    recon.clear();
    recon = multisetUnion(pos, neg);

    CPPUNIT_ASSERT_EQUAL(items.size(), plus.size() + minus.size());
    CPPUNIT_ASSERT(recon == allItems);
}