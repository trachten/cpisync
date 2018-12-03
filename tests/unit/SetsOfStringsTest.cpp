//
// Created by Bowen Song on 11/24/18.
//
#include "time.h"
#include "SetsOfStringsTest.h"
#include <climits>

CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfStringsTest);

SetsOfStringsTest::SetsOfStringsTest() = default;
SetsOfStringsTest::~SetsOfStringsTest() = default;

void SetsOfStringsTest::setUp() {
    const int SEED = 1;
    srand(SEED);
}

void SetsOfStringsTest::tearDown() {}

void SetsOfStringsTest::testAll() {
    SetsOfStrings Alice = SetsOfStrings(2,0,1,2); // default parameters
    string txt = randAsciiStr(8000);//randAsciiStr(1000000);
    Alice.injectString(txt);

    SetsOfStrings Bob = SetsOfStrings(2,0,1,2); // default parameters
    Bob.injectString(randStringEdit(txt,100));

    auto aliceset = Alice.getTerminalSubstr();
    auto bobset = Bob.getTerminalSubstr();

    cout<<"Number of different terminal str: "<<multisetDiff(aliceset,bobset).size()<<" from Alice:" <<aliceset.size()
    <<" and Bob: "<<bobset.size()<<endl;
}