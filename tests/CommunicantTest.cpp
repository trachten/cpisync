/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   CommunicantTest.cpp
 * Author: kaets
 *
 * Created on May 24, 2018, 10:08:52 AM
 */

#include "CommunicantTest.h"
#include "CommDummy.h"
#include "DataObject.h"
#include "Logger.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <climits>

CPPUNIT_TEST_SUITE_REGISTRATION(CommunicantTest);

CommunicantTest::CommunicantTest() {
}

CommunicantTest::~CommunicantTest() {
}

void CommunicantTest::setUp() {
    const int MY_SEED = 617; // a preset seed for pseudorandom number generation
    srand(MY_SEED); // seed the prng predictably so that the random numbers generated are predictable and reproducible
}

void CommunicantTest::tearDown() {
}

void CommunicantTest::testConstruct() {
    try {
        queue<char> qq;
        CommDummy c(&qq); // since every constructor calls their parent's empty constructors, we effectively test Communicant's empty constructor
    } catch(...) {
        CPPUNIT_FAIL("Expected no exceptions.");
    }
    
    // no exceptions thrown
    CPPUNIT_ASSERT(true);
}

void CommunicantTest::testBytesAndResetCommCounters() {
    queue<char> qq;
    
    // set up two communicants to either send or receive
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    long expXmitTot = 0;
    long expRecvTot = 0;
    
    for(int ii = 0; ii < TIMES; ii++) {
        string toSend = randString(LOWER_BOUND, UPPER_BOUND);
        long strLength = toSend.length();
        
        CPPUNIT_ASSERT_EQUAL(expXmitTot, cSend.getXmitBytesTot());
        CPPUNIT_ASSERT_EQUAL(expRecvTot, cRecv.getRecvBytesTot());
        
        cSend.resetCommCounters();
        cRecv.resetCommCounters();
        
        CPPUNIT_ASSERT_EQUAL(0l, cSend.getXmitBytes());
        CPPUNIT_ASSERT_EQUAL(0l, cRecv.getRecvBytes());
        
        cSend.commSend(toSend.data(), strLength);
        
        expXmitTot += strLength;
        
        CPPUNIT_ASSERT_EQUAL(strLength, cSend.getXmitBytes());
        CPPUNIT_ASSERT_EQUAL(expXmitTot, cSend.getXmitBytesTot());
        
        cRecv.commRecv(strLength);
        
        expRecvTot += strLength;
        
        CPPUNIT_ASSERT_EQUAL(strLength, cRecv.getRecvBytes());
        CPPUNIT_ASSERT_EQUAL(expRecvTot, cRecv.getRecvBytesTot());
    }
}

void CommunicantTest::testEstablishModAndCommZZ_p() {
    queue<char> qq;
    
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    for(int ii = 0; ii < TIMES; ii++) {
        ZZ_p::init(randZZ());
    
        // Tests that establishMod works with oneWay set to true.
        CPPUNIT_ASSERT(cSend.establishModSend(true));
        CPPUNIT_ASSERT(cRecv.establishModRecv(true));
        
        ZZ_p exp(rand());
        cSend.Communicant::commSend(exp);
        
        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_ZZ_p());
    }
}

void CommunicantTest::testCommUstringBytes() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    for(int ii = 0; ii < TIMES; ii++) {
        const ustring exp = (const unsigned char *) (randString(LOWER_BOUND, UPPER_BOUND).data());
        const int expLen = exp.size();

        // check that transmitted bytes are successfully incremented 
        const long before = cSend.getXmitBytes();
        cSend.Communicant::commSend(exp, expLen);
        const long after = cSend.getXmitBytes();

        CPPUNIT_ASSERT_EQUAL(0, exp.compare(cRecv.commRecv_ustring(expLen)));
        CPPUNIT_ASSERT_EQUAL(before + expLen, after);
    }
}

void CommunicantTest::testCommUstringNoBytes() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    for(int ii = 0; ii < TIMES; ii++) {
        const ustring exp = (const unsigned char *) (randString(LOWER_BOUND, UPPER_BOUND).data());
        cSend.Communicant::commSend(exp);
    
        CPPUNIT_ASSERT_EQUAL(0, exp.compare(cRecv.commRecv_ustring()));
    }
}

void CommunicantTest::testCommString() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    for(int ii = 0; ii < TIMES; ii++) {
        const string exp = randString(LOWER_BOUND, UPPER_BOUND);
        cSend.Communicant::commSend(exp);
        
        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_string());
    }
}

void CommunicantTest::testCommLong() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    for(int ii = 0; ii < TIMES; ii++){
        const long exp = randLong();
        cSend.Communicant::commSend(exp);
        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_long());
    }
}

void CommunicantTest::testCommDataObject() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    for(int ii = 0; ii < TIMES; ii++){
        DataObject exp(randString(LOWER_BOUND, UPPER_BOUND));
        cSend.Communicant::commSend(exp);
        
        CPPUNIT_ASSERT_EQUAL(exp.to_string(), cRecv.commRecv_DataObject()->to_string());
    }
}

void CommunicantTest::testCommDataObjectPriority() { // fix this test so that the repisint doesnt need to be changed
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    // Test case where RepIsInt is true
    DataObject::RepIsInt = true;

    for(int ii = 0; ii < TIMES; ii++) {

        DataObject exp(randIntString());
        exp.setPriority(randZZ());

        cSend.Communicant::commSend(exp, true);
        DataObject* res = cRecv.commRecv_DataObject_Priority();

        CPPUNIT_ASSERT_EQUAL(exp.to_string(), res->to_string());
        CPPUNIT_ASSERT_EQUAL(exp.getPriority(), res->getPriority());
    }

    // Test case where RepIsInt is false
    DataObject::RepIsInt = false;

    for(int ii = 0; ii < TIMES; ii++) {
        DataObject exp(randLong());
        exp.setPriority(randZZ());

        cSend.Communicant::commSend(exp, true);
        DataObject* res = cRecv.commRecv_DataObject_Priority();

        CPPUNIT_ASSERT_EQUAL(exp.to_string(), res->to_string());
        CPPUNIT_ASSERT_EQUAL(exp.getPriority(), res->getPriority());
    }
}

void CommunicantTest::testCommDataObjectList() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    const int TIMES = 50;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 10;

    for(int ii = 0; ii < TIMES; ii++){
        int length = randLenBetween(LOWER_BOUND, UPPER_BOUND);

        list<DataObject*> exp;
        for(int jj = 0; jj < length; jj++) {
            DataObject* dd = new DataObject(randZZ());
            exp.push_back(dd);
        }

        cSend.Communicant::commSend(exp);
        const list<DataObject*> res = cRecv.commRecv_DoList();
        // assert same length before iterating to check their equality
        CPPUNIT_ASSERT_EQUAL(exp.size(), res.size());

        list<DataObject*>::const_iterator expI = exp.begin();
        list<DataObject*>::const_iterator resI = res.begin();

        for(int jj = 0; jj < length; jj++) {
            DataObject currExp = **expI;
            DataObject currRes = **resI;
            CPPUNIT_ASSERT_EQUAL(currExp.to_string(), currRes.to_string());
            CPPUNIT_ASSERT_EQUAL(currExp.getPriority(), currRes.getPriority());
            // increment both iterators
            
            expI++;
            resI++;
        }
        for(DataObject* dop : exp)
            delete dop;
    }
}

void CommunicantTest::testCommDouble() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    // lower bound and upper bound for doubles; overrides the global constants
    const double LOWER_BOUND = 0;
    const double UPPER_BOUND = 5000;
    for(int ii = 0; ii < TIMES; ii++) {
        const double exp = randDouble(LOWER_BOUND, UPPER_BOUND);

        cSend.Communicant::commSend(exp);
   
        CPPUNIT_ASSERT_DOUBLES_EQUAL(exp, cRecv.commRecv_double(), DELTA);
    }
}

void CommunicantTest::testCommByte() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    for(int ii = 0; ii < TIMES; ii++) {
        const byte exp = randByte();
        cSend.Communicant::commSend(exp);

        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_byte());
    }
}

void CommunicantTest::testCommInt() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    for(int ii = 0; ii < TIMES; ii++) {
        const int exp = rand();
        cSend.Communicant::commSend(exp);

        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_int());
    }
}

void CommunicantTest::testCommVec_ZZ_p() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    for(int ii = 0; ii < TIMES; ii++) {
        ZZ_p::init(randZZ());
    
        // Tests that establishMod works with oneWay set to true.
        cSend.establishModSend(true);
        cRecv.establishModRecv(true);
        
        // pick a length in between lower and upper, inclusive
        int length = randLenBetween(LOWER_BOUND, UPPER_BOUND);
        
        vec_ZZ_p exp;
        for(int jj = 0; jj < length; jj++)
            exp.append(random_ZZ_p());
        
        cSend.Communicant::commSend(exp);
        
        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_vec_ZZ_p());
    }
}

void CommunicantTest::testCommZZ() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    for(int ii = 0; ii < TIMES; ii++) {
        const ZZ exp = randZZ();
        const int expSize = sizeof(exp);
        cSend.Communicant::commSend(exp, expSize);

        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_ZZ(expSize));
    }
}

void CommunicantTest::testCommZZNoArgs(){
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    for(int ii = 0; ii < TIMES; ii++) {
        const ZZ exp = randZZ();

        cSend.Communicant::commSend(exp);

        CPPUNIT_ASSERT_EQUAL(exp, cRecv.commRecv_ZZ());
    }
}
