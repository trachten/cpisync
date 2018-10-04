//
// Created by Bowen on 10/3/18.
//

#ifndef CPISYNCLIB_ADJMTXTEST_H
#define CPISYNCLIB_ADJMTXTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "AdjMtx.h"

class AdjMtxTest : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(AdjMtxTest);
        CPPUNIT_TEST(creatGraph);
        CPPUNIT_TEST(editGraph);
        CPPUNIT_TEST(allTest);
    CPPUNIT_TEST_SUITE_END();
public:
    AdjMtxTest();
    virtual ~AdjMtxTest();

    void creatGraph();
    void editGraph();
    void allTest();
};

#endif //CPISYNCLIB_ADJMTXTEST_H
