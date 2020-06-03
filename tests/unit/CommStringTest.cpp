/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   CommStringTest.cpp
 * Author: Eliezer Pearl
 *
 * Created on May 18, 2018, 10:52:33 AM
 */

#include "CommStringTest.h"
#include <CPISync/Communicants/CommString.h>

CPPUNIT_TEST_SUITE_REGISTRATION(CommStringTest);

CommStringTest::CommStringTest() = default;

CommStringTest::~CommStringTest() = default;

void CommStringTest::setUp() {
}

void CommStringTest::tearDown() {
}

void CommStringTest::testGetString() {
    const int TIMES = 50;
    for(int ii = 0; ii < TIMES; ii++) {
        const int LOWER = 0;
        const int UPPER = 10;

        // b64 case
        string init = randString(LOWER, UPPER);
        CommString cs(base64_encode(init, init.length()), true);
        CPPUNIT_ASSERT_EQUAL(init, cs.getString());

        // non-b64 case
        CommString ds(init, false);
        string s = ds.getString();
        CPPUNIT_ASSERT_EQUAL(init, ds.getString());
    }
}

void CommStringTest::testGetName() {
    CommString cs;
    const string name = "CommString";
    CPPUNIT_ASSERT_EQUAL(name, cs.getName());
}

void CommStringTest::testComm(){
    const int TIMES = 50;
    for(int ii = 0; ii < TIMES; ii++) {
        CommString cs;
        const int LOWER = 0;
        const int UPPER = 10;

        cs.commConnect(); // should do nothing
        cs.commListen(); // " "

        string toSend = randString(LOWER, UPPER);
        cs.commSend(toSend.data(), toSend.length());

        CPPUNIT_ASSERT_EQUAL((long) toSend.length(), cs.getXmitBytes());

        unsigned long byteNumRecv = randLenBetween(LOWER, toSend.length());

        CPPUNIT_ASSERT_EQUAL(toSend.substr(0, byteNumRecv), cs.commRecv(byteNumRecv));
        CPPUNIT_ASSERT_EQUAL(byteNumRecv, cs.getRecvBytes());

        cs.commClose();
    }

}
