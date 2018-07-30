//
// Created by eliez on 7/23/2018.
//

#ifndef CPISYNCLIB_GENSYNCTEST_H
#define CPISYNCLIB_GENSYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>
class GenSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(GenSyncTest);
    CPPUNIT_TEST(testAddRemoveElems);
    CPPUNIT_TEST(testGetName);
    CPPUNIT_TEST(testBuilder);
    CPPUNIT_TEST(testTwoWaySync);
    CPPUNIT_TEST(testOneWaySync);
    CPPUNIT_TEST(testAddSyncMethodAndComm);
    CPPUNIT_TEST(testCounters);
    CPPUNIT_TEST_SUITE_END();
public:
    GenSyncTest();
    virtual ~GenSyncTest();
    void setUp();
    void tearDown();
    void testBuilder();
    void testTwoWaySync();
    void testOneWaySync();
    void testGetName();
    void testAddRemoveElems();
    void testAddSyncMethodAndComm();
    void testCounters();
};


#endif //CPISYNCLIB_GENSYNCTEST_H
