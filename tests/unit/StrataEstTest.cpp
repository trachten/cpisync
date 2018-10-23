//
// Created by Bowen on 10/22/18.
//

#include <climits>
#include "StrataEstTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(StrataEstTest);

StrataEstTest::StrataEstTest() {};
StrataEstTest::~StrataEstTest() {};

void StrataEstTest::setUp() {
    const int seed = 1;
    srand(time(NULL));
}

void StrataEstTest::tearDown() {}

void StrataEstTest::createStrata() {
    const size_t SET_SIZE = 50000;
    const size_t SET_DIFF = 100;
    const size_t ELEM_SIZE = sizeof(DataObject *);
    // A - B = 50 items



    vector<ZZ> Items;
    StrataEst Alice = StrataEst(ELEM_SIZE);
    StrataEst Bob = StrataEst(ELEM_SIZE);
    for (int i = 0; i < SET_SIZE; ++i) {
        Items.push_back(randZZ());
    }

    for (int j = 0; j < SET_SIZE; ++j) {
        Alice.insert(new DataObject(Items[j]));
        if (j >= SET_DIFF){
            Bob.insert(new DataObject(Items[j]));
        }
    }

    Alice.exportStrata();
    Bob.exportStrata();

    auto est = (Alice -= Bob).estimate();

    CPPUNIT_ASSERT(est*1.5 > SET_DIFF);
    CPPUNIT_ASSERT(est < SET_DIFF + SET_DIFF);


}