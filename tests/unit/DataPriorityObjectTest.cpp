/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   DataObjectTest.cpp
 * Author: Ari Trachtenberg
 *
 * Created on August 3, 2018
 */

#include "DataPriorityObjectTest.h"
#include "DataPriorityObject.h"


using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(DataPriorityObjectTest);

void DataPriorityObjectTest::setUp(){
    srand(SEED);
}


void DataPriorityObjectTest::testPriority(){
    DataObject::RepIsInt = false;
    for(int ii = 0; ii < TIMES; ii++) {
        const ZZ priority = randZZ();
        DataPriorityObject dd;
        dd.setPriority(priority);
        CPPUNIT_ASSERT_EQUAL(priority, dd.getPriority());
    }
}

void DataPriorityObjectTest::testTimeStamp(){
    DataObject::RepIsInt = false;
    for(int ii = 0; ii < TIMES; ii++) {
        const clock_t time = (unsigned) randLong();
        DataPriorityObject dd;
        dd.setTimeStamp(time);
        CPPUNIT_ASSERT_EQUAL(dd.getTimeStamp(), time);
    }
}

void DataPriorityObjectTest::testToPriorityStringAndInitStringableAndSetPriority(){
    DataObject::RepIsInt = false;
// TODO: Improve this test once the to_string and to_priority_string docs are more specific

    for(int ii = 0; ii < TIMES; ii++) {
        ZZ init(rand());
        ZZ prio = randZZ();

        DataPriorityObject dd(init);
        dd.setPriority(prio);

        // TODO: Check that the priority and initial value were set correctly using to_priority_string
    }
}