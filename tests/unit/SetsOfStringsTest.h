//
// Created by Bowen Song on 11/24/18.
//

#ifndef CPISYNCLIB_SETSOFSTRINGSTEST_H
#define CPISYNCLIB_SETSOFSTRINGSTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "SetsOfStrings.h"
#include "Auxiliary.h"
#include <iostream>
#include <algorithm>

class SetsOfStringsTest : public CPPUNIT_NS::TestFixture{
CPPUNIT_TEST_SUITE(SetsOfStringsTest);
        CPPUNIT_TEST(testAll);
    CPPUNIT_TEST_SUITE_END();
public:
    SetsOfStringsTest();
    virtual ~SetsOfStringsTest();
    void setUp();
    void tearDown();

    void testAll();
};

#endif //CPISYNCLIB_SETSOFSTRINGSTEST_H
