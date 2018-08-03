/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   DataObjectTest.h
 * Author: kaets
 *
 * Created on May 15, 2018, 10:33:51 AM
 */

#ifndef DATAOBJECTTEST_H
#define DATAOBJECTTEST_H

#include <cppunit/extensions/HelperMacros.h>

class DataObjectTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(DataObjectTest);

    // Tests constructing a DataObject with a ZZ and DataObject::toZZ
    CPPUNIT_TEST(testToZZAndInitZZ);

    // Tests DataObject::to_string and constructing a DataObject with a string
    CPPUNIT_TEST(testToStringAndInitString);

    // Tests DataObject::to_string and constructing an empty DataObject
    CPPUNIT_TEST(testToStringAndInitEmpty);

    /**
     * Tests DataObject::to_priority_string, initializing a DataObject with a generic type that can be converted into a
     * string, and DataObject::setPriority.
     */
    CPPUNIT_TEST(testToPriorityStringAndInitStringableAndSetPriority);

    // Tests DataObject::to_char_array
    CPPUNIT_TEST(testToCharArray);

    // Tests DataObject::print
    CPPUNIT_TEST(testPrint);

    // Tests DataObject::operator<<
    CPPUNIT_TEST(testStreamInsertion);

    // Tests DataObject::operator<
    CPPUNIT_TEST(testLessThan);

    // Tests DataObject::getPriority
    CPPUNIT_TEST(testGetPriority);

    // Tests DataObject::setTimeStamp and DataObject::getTimeStamp
    CPPUNIT_TEST(testTimeStamp);

    CPPUNIT_TEST_SUITE_END();

public:
    const int TIMES = 50; // times to run a test
    const int LOWER = 0;
    const int UPPER = 50;

    DataObjectTest();
    virtual ~DataObjectTest();
    void setUp();
    void tearDown();

private:
    void testToZZAndInitZZ();
    void testToStringAndInitString();
    void testToStringAndInitEmpty();
    void testToPriorityStringAndInitStringableAndSetPriority();
    void testToCharArray();
    void testPrint();
    void testStreamInsertion();
    void testLessThan();
    void testGetPriority();
    void testTimeStamp();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataObjectTest, DataObjectTest );

#endif /* DATAOBJECTTEST_H */

