//
// Created by eliez on 7/23/2018.
//

#ifndef CPISYNCLIB_GENSYNCTEST_H
#define CPISYNCLIB_GENSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>
class GenSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(GenSyncTest);
    CPPUNIT_TEST(testSync);

    CPPUNIT_TEST_SUITE_END();
public:
    GenSyncTest();
    virtual ~GenSyncTest();
    void setUp();
    void tearDown();
    void testSync();
};


#endif //CPISYNCLIB_GENSYNCTEST_H
