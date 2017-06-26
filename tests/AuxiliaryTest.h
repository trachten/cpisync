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

    CPPUNIT_TEST(testStrToVecToStr);
//    CPPUNIT_TEST(testStrTo);
//    CPPUNIT_TEST(testToStr);
//    CPPUNIT_TEST(testWriteInts);
//    CPPUNIT_TEST(testMultisetPrint);
//    CPPUNIT_TEST(testMultisetIntersect);
//    CPPUNIT_TEST(testMultisetDiff);
//    CPPUNIT_TEST(testMultisetUnion);
//    CPPUNIT_TEST(testMultisetSubset);
//    CPPUNIT_TEST(testBase64_encode);
//    CPPUNIT_TEST(testBase64_decode);
//    CPPUNIT_TEST(testBase64_encode2);
//    CPPUNIT_TEST(testMin);

    CPPUNIT_TEST_SUITE_END();

public:
    AuxiliaryTest();
    virtual ~AuxiliaryTest();
    void setUp();
    void tearDown();

private:
    void testStrToVecToStr();
//    void testStrTo();
//    void testToStr();
//    void testWriteInts();
//    void testMultisetPrint();
//    void testMultisetIntersect();
//    void testMultisetDiff();
//    void testMultisetUnion();
//    void testMultisetSubset();
//    void testBase64_encode();
//    void testBase64_decode();
//    void testBase64_encode2();
//    void testMin();

};

#endif /* AUXILIARYTEST_H */

