//
// Created by eliez on 7/17/2018.
//

#ifndef CPISYNCLIB_IBLTTEST_H
#define CPISYNCLIB_IBLTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "IBLT.h"
#include <iostream>
#include <algorithm>

class IBLTTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(IBLTTest);
    CPPUNIT_TEST(simpleTest);
    CPPUNIT_TEST(testAddRemoveAndList);
    CPPUNIT_TEST_SUITE_END();
public:
    IBLTTest();
    virtual ~IBLTTest();
    void setUp();
    void tearDown();

    // Super simple and ugly test for the moment
    void simpleTest();

    void testAddRemoveAndList();
};


#endif //CPISYNCLIB_IBLTTEST_H
