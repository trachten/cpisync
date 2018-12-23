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

void IBLTTest::difTest(){
    vector<ZZ> items;
    const int SIZE = 500;
    const int diff = 35;
    const size_t ITEM_SIZE = sizeof(randZZ());
    for(int ii = 0; ii < SIZE; ii++) {
        auto tmp = randZZ();
        items.push_back(tmp);
    }

    IBLT ibltA(diff, ITEM_SIZE);
    IBLT ibltB(diff, ITEM_SIZE);
    for(int ii =diff; ii < SIZE; ii++) {
        ibltA.insert(items.at(ii), items.at(ii));
    }

    for(int ii=0; ii < SIZE; ii++) {
        ibltB.insert(items.at(ii), items.at(ii));
    }
    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(not ibltA.listEntries(plus,minus));
    plus.clear();
    minus.clear();
    CPPUNIT_ASSERT((ibltA-=ibltB).listEntries(plus,minus));

}

void IBLTTest::strTest(){
    vector<ZZ> items;
    size_t str_len = 11;
    const int SIZE = 500; // should be even
    const int diff = 35;
    const size_t ITEM_SIZE = sizeof(ZZ)*StrtoZZ(randAsciiStr(str_len)).size();
    for(int ii = 0; ii < SIZE; ii++) {
        ZZ tmp = StrtoZZ(randAsciiStr(str_len));
        items.push_back(tmp);
    }

    IBLT ibltA(diff, ITEM_SIZE);
    IBLT ibltB(diff, ITEM_SIZE);
    for(int ii =diff; ii < SIZE; ii++) {
        ibltA.insert(items.at(ii), items.at(ii));
    }

    for(int ii=0; ii < SIZE; ii++) {
        ibltB.insert(items.at(ii), items.at(ii));
    }
    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(not ibltA.listEntries(plus,minus));
    plus.clear();
    minus.clear();
    CPPUNIT_ASSERT((ibltA-=ibltB).listEntries(plus,minus));

}