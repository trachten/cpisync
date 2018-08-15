//
// Created by eliez on 7/23/2018.
//

#ifndef CPISYNCLIB_GENSYNCTEST_H
#define CPISYNCLIB_GENSYNCTEST_H

#include <string>
#include <cppunit/extensions/HelperMacros.h>

#include "Auxiliary.h"
#include "GenSync.h"

class GenSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(GenSyncTest);
    CPPUNIT_TEST(testAddRemoveElems);
    CPPUNIT_TEST(testGetName);
    CPPUNIT_TEST(testBuilder);
    CPPUNIT_TEST(testTwoWaySync);
    CPPUNIT_TEST(testOneWaySync);
    CPPUNIT_TEST(testAddRemoveSyncMethodAndComm);
    CPPUNIT_TEST(testCounters);
    CPPUNIT_TEST(testPort);
    CPPUNIT_TEST_SUITE_END();
public:
    GenSyncTest();

    ~GenSyncTest() override;
    void setUp() override;
    void tearDown() override;

    // Test that GenSyncs constructed using GenSync::Builder create equivalent GenSyncs to those constructed using the standard constructor
    void testBuilder();

    // Test syncinc GenSyncs with two-way SyncMethods
    void testTwoWaySync();

    // Test syncing GenSyncs with one-way SyncMethods
    void testOneWaySync();

    // Test that GenSync::getName returns some nonempty string
    void testGetName();

    // Test adding and removing data from GenSync
    void testAddRemoveElems();

    // Test adding and removing SyncMethods and Communicants from GenSync
    void testAddRemoveSyncMethodAndComm();

    // Test sync time and bytes-exchanged counters
    void testCounters();

    // Tests getPort
    void testPort();

};


#endif //CPISYNCLIB_GENSYNCTEST_H
