//
// Created by Bowen Song on 12/9/18.
//
#include "SetsOfContentTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfContentTest);

void SetsOfContentTest::setDiff() {

    string alicetxt = randAsciiStr(20000); // 20MB is top on MAC
    auto Alice = SetsOfContent(100);
    Alice.injectString(alicetxt);

    string bobtxt = randStringEdit(alicetxt, 10);
    auto Bob = SetsOfContent(100);
    Bob.injectString(bobtxt);


    CPPUNIT_ASSERT(Alice.retriveString() == bobtxt);


    //
}


void SetsOfContentTest::testAll() {
}