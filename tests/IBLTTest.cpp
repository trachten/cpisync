//
// Created by eliez on 7/17/2018.
//

#include <climits>
#include "IBLTTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IBLTTest);

ZZ randZZ() {
    return ZZ((((long) rand()) << CHAR_BIT) | rand());
}

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

void IBLTTest::simpleTest() {
    IBLT iblt(7);

    vector<pair<long, long>> items = {
            {53, 25},
            {85, 45},
            {92, 29},
            {34, 56},
            {134, 5235},
            {324546, 2341234},
            {324,245643}
    };

    for(auto pp : items) {
        iblt.insert(ZZ(pp.first), ZZ(pp.second));
    }
    cout << iblt.DumpTable() << endl;
    for(auto pp : items) {
        IBLT dup(iblt); // we need to duplicate in order to avoid the destructiveness
        ZZ resValue;
        CPPUNIT_ASSERT(dup.get(ZZ(pp.first), resValue));
        CPPUNIT_ASSERT_EQUAL(ZZ(pp.second), resValue);
    }

    iblt.erase(ZZ(1345215l), ZZ(523l));
    iblt.erase(ZZ(items[1].first), ZZ(items[1].second));

    vector<pair<ZZ, ZZ>> plus, minus;
    CPPUNIT_ASSERT(iblt.listEntries(plus, minus));
    for(auto pair : plus) {
        cout << "+{" << pair.first << "," << pair.second << "}";
    }

    cout << endl;
    for(auto pair : minus) {
        cout << "-{" << pair.first << "," << pair.second << "}";
    }
}

bool comp(const pair<ZZ, ZZ>& a, const pair<ZZ, ZZ>& b) {
    return a.first < b.first;
}

bool eq(const pair<ZZ, ZZ>& a, const pair<ZZ, ZZ>& b) {
    return a.first == b.first;
}

void IBLTTest::testAddRemoveAndList() {
    vector<pair<ZZ, ZZ>> items;
    const int SIZE = 50; // should be even
    for(int ii = 0; ii < SIZE; ii++) {
        items.push_back({randZZ(), randZZ()});
    }

    IBLT iblt(SIZE);
    for(int ii=0; ii < SIZE/2; ii++) {
        iblt.insert(items.at(ii).first, items.at(ii).second);
    }

    for(int ii=SIZE/2; ii < SIZE; ii++) {
        iblt.erase(items.at(ii).first, items.at(ii).second);
    }

    cout << iblt.DumpTable() << endl;

   // std::sort(items.begin(), items.end(), comp);
   // std::unique(items.begin(), items.end(), eq);

    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(iblt.listEntries(plus, minus));
    CPPUNIT_ASSERT_EQUAL(items.size(), plus.size() + minus.size());
}