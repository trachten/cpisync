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

    CPPUNIT_TEST(testToZZAndInitZZ);
    CPPUNIT_TEST(testToStringAndInitString);
    CPPUNIT_TEST(testToStringAndInitEmpty);
    CPPUNIT_TEST(testToCharArray);
    CPPUNIT_TEST(testPrint);
    CPPUNIT_TEST(testStreamInsertion);
    CPPUNIT_TEST(testLessThan);

    CPPUNIT_TEST_SUITE_END();

public:
    static const int TIMES = 50; // times to run a test
    static const int LOWER = 0;
    static const int UPPER = 50;

    DataObjectTest();

    ~DataObjectTest() override;
    void setUp() override;
    void tearDown() override;

private:
	/**
 	* Tests constructing a DataObject with a ZZ and DataObject::toZZ
 	*/
    static void testToZZAndInitZZ();

    /**
 	* Tests DataObject::to_string and constructing a DataObject from a random string
	 */
    static void testToStringAndInitString();

	/**
 	* Tests DataObject::to_string and constructing an empty DataObject
 	*/
    static void testToStringAndInitEmpty();

	/**
 	* Tests DataObject::to_char_array
 	*/
    static void testToCharArray();

	/**
 	* Tests DataObject::print
 	*/
    static void testPrint();

	/**
 	* Tests DataObject::operator<<
 	*/
    static void testStreamInsertion();

	/**
 	* Tests DataObject::operator<
 	*/
    static void testLessThan();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataObjectTest, DataObjectTest );

#endif /* DATAOBJECTTEST_H */

