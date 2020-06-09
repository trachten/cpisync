/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   CommStringTest.h
 * Author: Eliezer Pearl
 *
 * Created on May 18, 2018, 10:52:33 AM
 */

#ifndef COMMSTRINGTEST_H
#define COMMSTRINGTEST_H

#include <cppunit/extensions/HelperMacros.h>

class CommStringTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(CommStringTest);

    CPPUNIT_TEST(testGetString);
    CPPUNIT_TEST(testGetName);
    CPPUNIT_TEST(testComm);

    CPPUNIT_TEST_SUITE_END();

public:
    CommStringTest();

    ~CommStringTest() override;
    void setUp() override;
    void tearDown() override;

private:
	/**
 	* Tests that CommString::getString() correctly returns the string passed at construction
 	*/
    static void testGetString();

	/**
 	* Tests that CommString::getName() correctly returns "CommString"
 	*/
    static void testGetName();

	/**
 	* Tests a full round of communication
 	*/
    static void testComm();

};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CommStringTest, CommStringTest );

#endif /* COMMSTRINGTEST_H */

