/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.cpp
 * Author: kaets
 * 
 * Created on June 27, 2018, 9:49 AM
 */

#include "FullSyncTest.h"
#include "FullSync.h"
#include "CommSocket.h"
#include <pthread.h>

CPPUNIT_TEST_SUITE_REGISTRATION(FullSyncTest);

FullSyncTest::FullSyncTest() {
}

FullSyncTest::~FullSyncTest() {
}

void FullSyncTest::setUp() { 
}

void FullSyncTest::tearDown() { 
}


void *ggProcess(void* ){
    FullSync gg;
    CommSocket comm(4527, "localhost");
    list<DataObject*> selfMinusOther, otherMinusSelf;
    
    DataObject data[] = {
        DataObject(string("abc")),
        DataObject(string("def")),
        DataObject(string("ghj"))
    };

    for(DataObject& dd : data)
        gg.addElem(&dd);


    gg.SyncServer(&comm, selfMinusOther, otherMinusSelf);
    
    long unsigned int expLen = 1;
    
    // expected 1 difference per diff list
    CPPUNIT_ASSERT_EQUAL(expLen, selfMinusOther.size());
    CPPUNIT_ASSERT_EQUAL(expLen, otherMinusSelf.size());
    
    DataObject expectedSMO(string("ghj"));
    DataObject expectedOMS(string("ghi"));
    // since each diff list only has one element, just check the head of the list
    CPPUNIT_ASSERT(!(expectedSMO < *selfMinusOther.front() || *selfMinusOther.front() < expectedSMO)); // tests equality
    CPPUNIT_ASSERT(!(expectedOMS < *otherMinusSelf.front() || *otherMinusSelf.front() < expectedOMS)); // tests equality
    
}

void FullSyncTest::testAll() {
    pthread_t ggProc;
    pthread_create(&ggProc, nullptr, ggProcess, nullptr);

    FullSync ff;
    CommSocket comm(4527, "localhost");
    list<DataObject*> selfMinusOther, otherMinusSelf;
    
    DataObject data[] = {
        DataObject(string("abc")),
        DataObject(string("def")),
        DataObject(string("ghi"))
    };
  
    for(DataObject& dd : data)
        ff.addElem(&dd);
    
    ff.SyncClient(&comm, selfMinusOther, otherMinusSelf);
    pthread_join(ggProc, nullptr);
    
    DataObject expectedSMO(string("ghi"));
    DataObject expectedOMS(string("ghj"));
    // since each diff list only has one element, just check the head of the list
    CPPUNIT_ASSERT(!(expectedSMO < *selfMinusOther.front() || *selfMinusOther.front() < expectedSMO)); // tests equality
    CPPUNIT_ASSERT(!(expectedOMS < *otherMinusSelf.front() || *otherMinusSelf.front() < expectedOMS)); // tests equality
}

