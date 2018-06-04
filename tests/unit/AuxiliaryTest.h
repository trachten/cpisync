/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   AuxiliaryTest.h
 * Author: trachten
 *
 * Created on Jun 21, 2017, 4:17:10 PM
 */

#ifndef AUXILIARYTEST_H
#define AUXILIARYTEST_H

#include <cppunit/extensions/HelperMacros.h>

class AuxiliaryTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(AuxiliaryTest);

    CPPUNIT_TEST(testStrTo);
    CPPUNIT_TEST(testToStr);
    CPPUNIT_TEST(testBase64_encode);
    CPPUNIT_TEST(testBase64_decode);
    CPPUNIT_TEST(testStrToVecToStr);
    // CPPUNIT_TEST(testMultisetIntersect);
    // CPPUNIT_TEST(testMultisetDiff);
    // CPPUNIT_TEST(testMultisetUnion);
    // CPPUNIT_TEST(testMultisetSubset); 

    CPPUNIT_TEST_SUITE_END();

public:
    AuxiliaryTest();

    ~AuxiliaryTest() override;
    void setUp() override;
    void tearDown() override;

private:
    void testStrTo();
    void testToStr();
    void testBase64_encode();
    void testBase64_decode();
    void testStrToVecToStr();
    // void testMultisetIntersect();
    // void testMultisetDiff();
    // void testMultisetUnion();
    // void testMultisetSubset();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( AuxiliaryTest, AuxiliaryTest );

#endif /* AUXILIARYTEST_H */

