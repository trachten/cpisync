//
// Created by Bowen Song on 10/30/18.
//

#ifndef CPISYNCLIB_CPITEST_H
#define CPISYNCLIB_CPITEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "CPISync.h"
#include "CPI.h"
#include "Auxiliary.h"

class CPITest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(CPITest);
    CPPUNIT_TEST(CPISyncTest);
    CPPUNIT_TEST_SUITE_END();
public:
    CPITest(){};
    virtual ~CPITest(){};
    void CPISyncTest();
};

#endif //CPISYNCLIB_CPITEST_H
