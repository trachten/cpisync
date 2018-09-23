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
    string Alicetxt = genRandString(100);  // generate a string
    string Bobtxt = randStringEdit(Alicetxt,10);  // Generate a edited string
    K_Shingle Alice = K_Shingle(Alicetxt, 3);  // Init k shingling on Alice
    auto AliceStringNOrder = Alice.reconstructStringBacktracking();  // Get order of the cycle

    CPPUNIT_ASSERT(AliceStringNOrder.first=="$"+Alicetxt+"$");  // Make sure Alice can recover her own string from shingle set
    CPPUNIT_ASSERT(AliceStringNOrder.second > -1);  //Make sure string order is not -1 (default)


}