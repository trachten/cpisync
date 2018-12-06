//
// Created by Bowen Song on 9/16/18.
//

#include <climits>
#include "kshinglingTest.h"
#include "time.h"


CPPUNIT_TEST_SUITE_REGISTRATION(kshinglingTest);

kshinglingTest::kshinglingTest() {
}

kshinglingTest::~kshinglingTest() {
}

void kshinglingTest::setUp() {
    const int SEED = 617;
    srand(SEED);
}

void kshinglingTest::tearDown() {
}

void kshinglingTest::testAll() {
    // init a string of random byte (shortest,longest) possible string len
    string Alicetxt = randSampleTxt(500);  // generate a string, no longer than 1e4
    //string Bobtxt = randStringEdit(Alicetxt,10);  // Generate a edited string

    clock_t t1 = clock();
    int shingle_size = ceil(log2(Alicetxt.size()));
    K_Shingle Alice = K_Shingle(shingle_size);

    Alice.inject(Alicetxt);

    auto AliceStringNOrder = Alice.reconstructStringBacktracking();  // Get order of the cycle
    clock_t t2 = clock();

    cout<<to_string(double(t2-t1)/CLOCKS_PER_SEC)<<endl;

    //test functions
    CPPUNIT_ASSERT(AliceStringNOrder.first==Alicetxt);  // Make sure Alice can recover her own string from shingle set
    CPPUNIT_ASSERT(AliceStringNOrder.second > 0);  //Make sure string order is not 0 (default)
    //CPPUNIT_ASSERT(Alicetxt!=Bobtxt);

    //auto RedoStringNOrder = Alice.reconstructStringBacktracking(AliceStringNOrder.second);  // Get string through the order of cycle
    //CPPUNIT_ASSERT(RedoStringNOrder.first==Alicetxt);  // Make sure Alice can recover her own string from shingle set
    //CPPUNIT_ASSERT(RedoStringNOrder.second ==AliceStringNOrder.second);  //Make sure string order returned are the same

    CPPUNIT_ASSERT(Alice.getOriginString()==AliceStringNOrder.first);
    CPPUNIT_ASSERT(Alice.getShingleSet().size()>0);


    //test with Half round sync

}