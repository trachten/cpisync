//
// Created by Bowen on 9/24/18.
//

#ifndef CPISYNCLIB_KSHINGLINGSYNCTEST_H
#define CPISYNCLIB_KSHINGLINGSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kshinglingSync.h"


class kshinglingSyncTest : public CPPUNIT_NS::TestFixture{
  CPPUNIT_TEST_SUITE(kshinglingSyncTest);
  CPPUNIT_TEST(testAll);
  CPPUNIT_TEST_SUITE_END();
public:
    kshinglingSyncTest();
    virtual ~kshinglingSyncTest();
    void setUp();
    void tearDown();

    void testAll();
};

#endif //CPISYNCLIB_KSHINGLINGSYNCTEST_H
