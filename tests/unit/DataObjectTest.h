/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   DataObjectTest.h
 * Author: Eliezer Pearl
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

    // Tests DataObject::to_char_array
    CPPUNIT_TEST(testToCharArray);

    // Tests DataObject::print
    CPPUNIT_TEST(testPrint);

    // Tests DataObject::operator<<
    CPPUNIT_TEST(testStreamInsertion);

    // Tests DataObject::operator<
    CPPUNIT_TEST(testLessThan);

    CPPUNIT_TEST_SUITE_END();

public:
    const int TIMES = 50; // times to run a test
    const int LOWER = 0;
    const int UPPER = 50;

    DataObjectTest();

    ~DataObjectTest() override;
    void setUp() override;
    void tearDown() override;

private:
	/**
 	* Tests constructing a DataObject with a ZZ and DataObject::toZZ
 	*/
    void testToZZAndInitZZ();

	/**
 	* Tests DataObject::to_string and constructing a DataObject with a string
 	*/
    void testToStringAndInitString();

	/**
 	* Tests DataObject::to_string and constructing an empty DataObject
 	*/
    static void testToStringAndInitEmpty();

	/**
 	* Tests DataObject::to_char_array
 	*/
    void testToCharArray();

	/**
 	* Tests DataObject::print
 	*/
    void testPrint();

	/**
 	* Tests DataObject::operator<<
 	*/
    void testStreamInsertion();

	/**
 	* Tests DataObject::operator<
 	*/
    void testLessThan();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataObjectTest, DataObjectTest );

#endif /* DATAOBJECTTEST_H */

