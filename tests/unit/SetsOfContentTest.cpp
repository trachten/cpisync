//
// Created by Bowen Song on 12/9/18.
//
#include "SetsOfContentTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfContentTest);

void SetsOfContentTest::setDiff() {
    string alicetxt = randSampleTxt(10000);
    auto Alice = SetsOfContent(100);
    Alice.injectString(alicetxt);
}


void SetsOfContentTest::testAll() {
}