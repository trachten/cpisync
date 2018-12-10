//
// Created by Bowen Song on 12/9/18.
//

#ifndef CPISYNCLIB_SETSOFCONTENTTEST_H
#define CPISYNCLIB_SETSOFCONTENTTEST_H


#include <cppunit/extensions/HelperMacros.h>
#include "SetsOfContent.h"


class SetsOfContentTest : public CPPUNIT_NS::TestFixture{

CPPUNIT_TEST_SUITE(SetsOfContentTest);
        CPPUNIT_TEST(setDiff);
        CPPUNIT_TEST(testAll);
    CPPUNIT_TEST_SUITE_END();
public:

    SetsOfContentTest() = default;
    virtual ~SetsOfContentTest() = default;
    void setUp(){
        const int SEED = 617;
        srand(SEED);
    };

    void testAll();
    void setDiff();
};


#endif //CPISYNCLIB_SETSOFCONTENTTEST_H
