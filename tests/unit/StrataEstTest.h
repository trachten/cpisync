//
// Created by Bowen on 10/22/18.
//

#ifndef CPISYNCLIB_STRATAESTTEST_H
#define CPISYNCLIB_STRATAESTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "StrataEst.h"
#include "Auxiliary.h"
#include <iostream>
#include <algorithm>

class StrataEstTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(StrataEstTest);
    CPPUNIT_TEST(createStrata);
    CPPUNIT_TEST_SUITE_END();
public:
    StrataEstTest();
    virtual ~StrataEstTest();
    void setUp();
    void tearDown();

    // testing to create strata in different presettings
    void createStrata();
};

#endif //CPISYNCLIB_STRATAESTTEST_H
