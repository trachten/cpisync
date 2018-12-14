//
// Created by Bowen Song on 12/9/18.
//
#include "SetsOfContentTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfContentTest);

void SetsOfContentTest::setDiff() {
    string alicetxt = randSampleTxt(40000);
    auto Alice = SetsOfContent(100,3);
    Alice.injectString(alicetxt);
    CPPUNIT_ASSERT(Alice.retriveString() == alicetxt);
    string bobtxt = randStringEdit(alicetxt,10);
    auto Bob = SetsOfContent(100,3);
    Bob.injectString(bobtxt);
    CPPUNIT_ASSERT(Bob.retriveString() == bobtxt);


    //
}


void SetsOfContentTest::testAll() {
}