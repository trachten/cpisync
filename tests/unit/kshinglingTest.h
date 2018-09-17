//
// Created by Bowen Song on 9/16/18.
//

#ifndef CPISYNCLIB_KSHINGLINGTEST_H
#define CPISYNCLIB_KSHINGLINGTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kshingling.h"
#include "Auxiliary.h"
#include <iostream>
#include <algorithm>

class kshinglingTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(kshinglingTest);
    CPPUNIT_TEST(testAll);
    CPPUNIT_TEST_SUITE_END();
public:
    kshinglingTest();
    virtual ~kshinglingTest();
    void setUp();
    void tearDown();

    // Tests every function in IBLT together, except for size
    void testAll();
};

#endif //CPISYNCLIB_KSHINGLINGTEST_H
