/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 7/17/2018.
//

#ifndef CPISYNCLIB_IBLTTEST_H
#define CPISYNCLIB_IBLTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "Syncs/IBLT.h"
#include "Aux/Auxiliary.h"
#include <iostream>
#include <algorithm>

class IBLTTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(IBLTTest);
    CPPUNIT_TEST(testAll);
    CPPUNIT_TEST_SUITE_END();
public:
    IBLTTest();
    virtual ~IBLTTest();
    void setUp();
    void tearDown();

    // Tests every function in IBLT together, except for size
    void testAll();
};


#endif //CPISYNCLIB_IBLTTEST_H
