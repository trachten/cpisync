//
// Created by Bowen Song on 9/16/18.
//

#include <climits>
#include "kshinglingTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(kshinglingTest);

kshinglingTest::kshinglingTest() {
}

kshinglingTest::~kshinglingTest() {
}

void kshinglingTest::setUp() {
}

void kshinglingTest::tearDown() {
}

void kshinglingTest::testAll() {
    // init a string of random byte (shortest,longest) possible string len
    string Alicetxt = randAsciiStr(100);  // generate a string
    string Bobtxt = randStringEdit(Alicetxt,10);  // Generate a edited string

    K_Shingle Alice = K_Shingle(3);
    Alice.create(Alicetxt);  // Init k shingling on Alice

    auto AliceStringNOrder = Alice.reconstructStringBacktracking();  // Get order of the cycle

    //test functions
    CPPUNIT_ASSERT(AliceStringNOrder.first==Alicetxt);  // Make sure Alice can recover her own string from shingle set
    CPPUNIT_ASSERT(AliceStringNOrder.second > -1);  //Make sure string order is not -1 (default)
    CPPUNIT_ASSERT(Alicetxt!=Bobtxt);

    auto RedoStringNOrder = Alice.reconstructStringBacktracking(AliceStringNOrder.second);  // Get string through the order of cycle
    CPPUNIT_ASSERT(RedoStringNOrder.first==Alicetxt);  // Make sure Alice can recover her own string from shingle set
    CPPUNIT_ASSERT(RedoStringNOrder.second ==AliceStringNOrder.second);  //Make sure string order returned are the same

    CPPUNIT_ASSERT(Alice.getOriginString()==AliceStringNOrder.first);
    CPPUNIT_ASSERT(Alice.getShingleSet().size()>0);


    //test with Half round sync

}