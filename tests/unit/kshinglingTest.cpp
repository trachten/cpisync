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
//    const int SEED = 617;
//    srand(SEED);
}

void kshinglingTest::tearDown() {
}

void kshinglingTest::testAll() {
    // init a string of random byte (shortest,longest) possible string len
    string Alicetxt = genRandString(10);
    string Bobtxt = randStringEdit(Alicetxt,3);
    K_Shingle Alice = K_Shingle(2, "bowen");

}