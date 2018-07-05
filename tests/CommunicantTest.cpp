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
#include <math.h>
#include <stdlib.h>
#include <limits.h>

CPPUNIT_TEST_SUITE_REGISTRATION(CommunicantTest);

inline int randLenBetween(int lower, int upper) {
    int length = (rand() % (upper + 1));
    if(length < lower) length = lower;
    return length;
}

inline long randLong() {
    if(sizeof(int) < sizeof(long)) {
        return static_cast<long>(rand());
    } else {
        return (static_cast<long>(rand()) << (sizeof(int) * CHAR_BIT)) | rand(); // lshift the amount of bits in an int and then bitwise or a random int
    }
}

inline byte randByte() {
    return (char) (rand() % 256);
}


inline string randString(int lower, int upper) {
    stringstream str;

    // pick a length in between lower and upper, inclusive
    int length = randLenBetween(lower, upper);

    for(int jj = 0; jj < length; jj++)
        str << randByte(); // generate a random character and add to stringstream

    return str.str();
}

inline string randIntString() {
    return to_string(rand());
}

inline double randDouble(double lower, double upper) {
    return ( ( double )rand() * ( upper - lower ) ) / ( double )RAND_MAX + lower;
}

CommunicantTest::CommunicantTest() {
}

CommunicantTest::~CommunicantTest() {
}

void CommunicantTest::setUp() {
    const int MY_SEED = 123456; // a preset seed for pseudorandom number generation
    srand(MY_SEED); // seed the prng predictably so that the random numbers generated are predictable and reproducible
}

void CommunicantTest::tearDown() {
}

void CommunicantTest::testConstruct() {
    try {
        queue<char> qq;
        CommDummy c(&qq); // since every constructor calls their ancestor's empty constructors, we effectively test Communicant's empty constructor
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
    const long int NONE = 0;
    const int SIZE = 100;
    const int LOWER_BOUND = 0;
    const int UPPER_BOUND = 50;
    
    long int expXmitTot = 0;
    long int expRecvTot = 0;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const string TO_SEND = randString(LOWER_BOUND, UPPER_BOUND);
        const long int STR_LENGTH = TO_SEND.length();
        
        CPPUNIT_ASSERT_EQUAL(expXmitTot, cSend.getXmitBytesTot());
        CPPUNIT_ASSERT_EQUAL(expRecvTot, cRecv.getRecvBytesTot());
        
        cSend.resetCommCounters();
        cRecv.resetCommCounters();
        
        CPPUNIT_ASSERT_EQUAL(NONE, cSend.getXmitBytes());
        CPPUNIT_ASSERT_EQUAL(NONE, cRecv.getRecvBytes());
        
        cSend.commSend(TO_SEND.data(), STR_LENGTH);
        
        expXmitTot += STR_LENGTH;
        
        CPPUNIT_ASSERT_EQUAL(STR_LENGTH, cSend.getXmitBytes());
        CPPUNIT_ASSERT_EQUAL(expXmitTot, cSend.getXmitBytesTot());
        
        cRecv.commRecv(STR_LENGTH);
        
        expRecvTot += STR_LENGTH;
        
        CPPUNIT_ASSERT_EQUAL(STR_LENGTH, cRecv.getRecvBytes());
        CPPUNIT_ASSERT_EQUAL(expRecvTot, cRecv.getRecvBytesTot());
    }
}

void CommunicantTest::testEstablishModAndCommZZ_p() {
    queue<char> qq;
    
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    // Generate two sets of 100 random mods and ZZ_p values
    const int SIZE = 100;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const ZZ MOD(rand());
        ZZ_p::init(MOD);
    
        // Tests that establishMod works with oneWay set to true.
        const bool ONE_WAY = true;
        CPPUNIT_ASSERT(cSend.establishModSend(ONE_WAY)); 
        CPPUNIT_ASSERT(cRecv.establishModRecv(ONE_WAY));
        
        const ZZ_p EXP(rand());
        cSend.Communicant::commSend(EXP);
        const ZZ_p RES = cRecv.commRecv_ZZ_p();
        
        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}

void CommunicantTest::testCommUstringBytes() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 100;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 250; // arbitrary upper bound
    
    for(int ii = 0; ii < SIZE; ii++) {
        const string SS = randString(LOWER_BOUND, UPPER_BOUND);
        const ustring EXP = (const unsigned char *) (SS.data());
        const int EXP_LEN = EXP.size();

        // check that transmitted bytes are successfully incremented 
        const long int BEFORE = cSend.getXmitBytes();
        cSend.Communicant::commSend(EXP, EXP_LEN);
        const long int AFTER = cSend.getXmitBytes();

        const ustring RES = cRecv.commRecv_ustring(EXP_LEN);

        CPPUNIT_ASSERT_EQUAL(EXP.compare(RES), 0);
        CPPUNIT_ASSERT_EQUAL(BEFORE + EXP_LEN, AFTER);
    }
}

void CommunicantTest::testCommUstringNoBytes() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 100;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 25; // arbitrary upper bound
    
    for(int ii = 0; ii < SIZE; ii++) {
        const string SS = randString(LOWER_BOUND, UPPER_BOUND);
        const ustring EXP = (const unsigned char *) (SS.data());
        cSend.Communicant::commSend(EXP);
        const ustring RES = cRecv.commRecv_ustring();
    
        CPPUNIT_ASSERT_EQUAL(EXP.compare(RES), 0);
    }
}

void CommunicantTest::testCommString() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 100;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 25;
    
    
    for(int ii = 0; ii < SIZE; ii++) {
        const string EXP = randString(LOWER_BOUND, UPPER_BOUND);
        cSend.Communicant::commSend(EXP);
        const string RES = cRecv.commRecv_string();
        
        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}

void CommunicantTest::testCommLong() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 100;
    
    for(int ii = 0; ii < SIZE; ii++){
        const long EXP = randLong();
        cSend.Communicant::commSend(EXP);
        const long RES = cRecv.commRecv_long();
        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}

void CommunicantTest::testCommDataObject() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    const int SIZE = 100;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 25;
    
    for(int ii = 0; ii < SIZE; ii++){
        const string SS = randString(LOWER_BOUND, UPPER_BOUND);
        DataObject exp(SS);
        cSend.Communicant::commSend(exp);
        DataObject* res = cRecv.commRecv_DataObject();
        
        // same as testing for equality.
        CPPUNIT_ASSERT(!(exp < *res || *res < exp));
    }
}

void CommunicantTest::testCommDataObjectPriority() { // fix this test so that the repisint doesnt need to be changed
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    const bool SEND_PRIORITY = true;
    
    const int SIZE = 100;
    
    // Test case where RepIsInt is true
    DataObject::RepIsInt = true;

    for(int ii = 0; ii < SIZE; ii++) {
        const ZZ PRIORITY(randLong());
        DataObject exp(randIntString());
        exp.setPriority(PRIORITY);
        
        cSend.Communicant::commSend(exp, SEND_PRIORITY);
        DataObject* res = cRecv.commRecv_DataObject_Priority();
        // same as testing for equality.
        CPPUNIT_ASSERT(!(exp < *res || *res < exp));
        CPPUNIT_ASSERT_EQUAL(exp.getPriority(), res->getPriority());
    }
    
    // Test case where RepIsInt is false
    DataObject::RepIsInt = false;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const ZZ PRIORITY(randLong());
        DataObject exp(randLong());
        exp.setPriority(PRIORITY);
        
        cSend.Communicant::commSend(exp, SEND_PRIORITY);
        DataObject* res = cRecv.commRecv_DataObject_Priority();
        // same as testing for equality.
        CPPUNIT_ASSERT(!(exp < *res || *res < exp));
        CPPUNIT_ASSERT_EQUAL(exp.getPriority(), res->getPriority());
    }
}

void CommunicantTest::testCommDataObjectList() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 50;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 10;
    
    for(int ii = 0; ii < SIZE; ii++){
        int length = randLenBetween(LOWER_BOUND, UPPER_BOUND);
        
        list<DataObject*> exp;
        for(int jj = 0; jj < length; jj++) {
            DataObject* dd = new DataObject(randLong());
            exp.push_back(dd);
        }
        
        cSend.Communicant::commSend(exp);
        const list<DataObject*> RES = cRecv.commRecv_DoList();
        // assert same length before iterating to check their equality
        CPPUNIT_ASSERT_EQUAL(exp.size(), RES.size());
    
        list<DataObject*>::const_iterator expI = exp.begin();
        list<DataObject*>::const_iterator resI = RES.begin();
    
        for(int jj = 0; jj < length; jj++) {
            DataObject currExp = **expI;
            DataObject currRes = **resI;
            CPPUNIT_ASSERT(!(currExp < currRes || currRes < currExp)); // equality test for each DataObject
            CPPUNIT_ASSERT_EQUAL(currExp.getPriority(), currRes.getPriority());
            // increment both iterators
            
            advance(expI, 1);
            advance(resI, 1);
        }
        for(DataObject* dop : exp)
            delete dop;
    }
}

void CommunicantTest::testCommDouble() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int DELTA = 0.00001; // error tolerance comparing doubles
    
    const int SIZE = 100;
    const double LOWER_BOUND = 0;
    const double UPPER_BOUND = 5000;
    for(int ii = 0; ii < SIZE; ii++) {
        const double EXP = randDouble(LOWER_BOUND, UPPER_BOUND);

        cSend.Communicant::commSend(EXP);
        const double RES = cRecv.commRecv_double();
   
        CPPUNIT_ASSERT_DOUBLES_EQUAL(EXP, RES, DELTA);
    }
}

void CommunicantTest::testCommByte() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 100;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const byte EXP = randByte();
        cSend.Communicant::commSend(EXP);
        const byte RES = cRecv.commRecv_byte();

        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}

void CommunicantTest::testCommInt() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);
    
    const int SIZE = 100;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const int EXP = rand();
        cSend.Communicant::commSend(EXP);
        const int RES = cRecv.commRecv_int();

        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}

void CommunicantTest::testCommVec_ZZ_p() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    // Generate two sets of 100 random mods and veczzp values
    const int SIZE = 100;
    const int LOWER_BOUND = 1;
    const int UPPER_BOUND = 10;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const ZZ MOD(rand());
        ZZ_p::init(MOD);
    
        // Tests that establishMod works with oneWay set to true.
        const bool ONE_WAY = true;
        cSend.establishModSend(ONE_WAY);
        cRecv.establishModRecv(ONE_WAY);
        
        // pick a length in between lower and upper, inclusive
        int length = randLenBetween(LOWER_BOUND, UPPER_BOUND);
        
        vec_ZZ_p exp;
        for(int jj = 0; jj < length; jj++)
            exp.append(ZZ_p(randLong()));
        
        cSend.Communicant::commSend(exp);
        const vec_ZZ_p RES = cRecv.commRecv_vec_ZZ_p();
        
        CPPUNIT_ASSERT_EQUAL(exp, RES);
    }
}

void CommunicantTest::testCommZZ() {
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    const int SIZE = 100;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const ZZ EXP(randLong());
        const int EXP_SIZE = sizeof(EXP);

        cSend.Communicant::commSend(EXP, EXP_SIZE);
        const ZZ RES = cRecv.commRecv_ZZ(EXP_SIZE);

        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}

void CommunicantTest::testCommZZNoArgs(){
    queue<char> qq;
    CommDummy cSend(&qq);
    CommDummy cRecv(&qq);

    const int SIZE = 100;
    
    for(int ii = 0; ii < SIZE; ii++) {
        const ZZ EXP(randLong());

        cSend.Communicant::commSend(EXP);
        const ZZ RES = cRecv.commRecv_ZZ();

        CPPUNIT_ASSERT_EQUAL(EXP, RES);
    }
}
