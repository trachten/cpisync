/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   AuxiliaryTest.h
 * Author: Ari Trachtenberg
 *
 * Created on Jun 21, 2017, 4:17:10 PM
 */

#ifndef AUXILIARYTEST_H
#define AUXILIARYTEST_H

#include <cppunit/extensions/HelperMacros.h>

class AuxiliaryTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(AuxiliaryTest);
	CPPUNIT_TEST(testSplit);
	CPPUNIT_TEST(testStrTo);
	CPPUNIT_TEST(testToStr);
	CPPUNIT_TEST(testBase64_encode);
	CPPUNIT_TEST(testBase64_decode);
	CPPUNIT_TEST(testStrToVecToStr);
	CPPUNIT_TEST(testMultisetIntersect);
	CPPUNIT_TEST(testMultisetDiff);
	CPPUNIT_TEST(testMultisetUnion);
	CPPUNIT_TEST(testMultisetSubset);

	CPPUNIT_TEST_SUITE_END();

  public:
	AuxiliaryTest();

	~AuxiliaryTest() override;
	void setUp() override;
	void tearDown() override;

  private:
	/**
	 * Tests casting a string to int, double and ZZ
	 */
	static void testStrTo();

	/**
	 * Test casting int double and ZZ to string
	 */
	static void testToStr();

	static void testSplit();

	/**
	 * Tests encoding strings to base64 (Only use 0-9,a-z,A-Z and +/) {Equals sign is used as padding}
	 */
	static void testBase64_encode();

	/**
	 * Tests decoding strings from base64 back to their original values
	 */
	static void testBase64_decode();

	/**
	 * Tests that str to vec and vec to string work properly (by reversing each other)
	 */
	static void testStrToVecToStr();

	/**
	 * Tests that multiset intersect correctly identifies which elements are in both sets (Intersection)
	 */
	static void testMultisetIntersect();

	/**
	 * Tests that multisetDiff correctly identifies elements that are in only one of the two given sets (Difference)
	 */
	static void testMultisetDiff();

	/**
	 * Tests that multisetUnion correctly identifies elements that are in set A or set B (Union)
	 */
	static void testMultisetUnion();

	/**
	 * Tests that MultisetSubset correctly returns a subset of the correct size from the given multiset
	 */
	static void testMultisetSubset();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AuxiliaryTest, AuxiliaryTest);

#endif /* AUXILIARYTEST_H */
