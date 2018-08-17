/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   DataObjectTest.h
 * Author: Eliezer Pearl
 *
 * Created on May 15, 2018, 10:33:51 AM
 */

#ifndef DATAOBJECTPRIORITYTEST_H
#define DATAOBJECTPRIORITYTEST_H

#include <cppunit/extensions/HelperMacros.h>

class DataPriorityObjectTest : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(DataPriorityObjectTest);

        /**
         * Tests DataObject::to_priority_string, initializing a DataObject with a generic type that can be converted into a
         * string, and DataObject::setPriority.
         */
        CPPUNIT_TEST(testToPriorityStringAndInitStringableAndSetPriority);

        // Tests DataObject::getPriority

            // Tests DataObject::getPriority
            CPPUNIT_TEST(testPriority);

            // Tests DataObject::setTimeStamp and DataObject::getTimeStamp
            CPPUNIT_TEST(testTimeStamp);

    CPPUNIT_TEST_SUITE_END();

public:
    const int TIMES = 50; // times to run a test
    const int LOWER = 0;
    const int UPPER = 50;

    DataPriorityObjectTest() = default;

    //virtual ~DataPriorityObjectTest();
    void setUp() override;
    void tearDown() override {}

private:
    void testToPriorityStringAndInitStringableAndSetPriority();
    void testPriority();
    void testTimeStamp();

    const int SEED = 617;
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataPriorityObjectTest, DataPriorityObjectTest );

#endif /* DATAOBJECTTEST_H */

