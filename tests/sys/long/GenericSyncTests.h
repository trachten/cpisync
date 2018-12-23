/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/23/2018.
//

#ifndef CPISYNCLIB_GENSYNCTESTS_H
#define CPISYNCLIB_GENSYNCTESTS_H

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
    CPPUNIT_TEST(testTwoWayProbSync);
    //CPPUNIT_TEST(testOneWaySync); TODO: Fix forkHandle s.t. server uses listenSync in parent, client uses startSync in child
    //CPPUNIT_TEST(testOneWayProbSync); TODO: " "
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

    // Test syncing GenSyncs with two-way SyncMethods
    void testTwoWaySync();

    // Test syncing GenSyncs with two-way SyncMethods that have a chance of only partly reconciling
    void testTwoWayProbSync();

    // Test syncing GenSyncs with one-way SyncMethods
    void testOneWaySync();

    // Test syncing GenSyncs with one-way SyncMethods that have a chance of only partly reconciling
    void testOneWayProbSync();

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

private:
    // constants
    const unsigned int ELTS = 64; // amount of elements to be added to the GenSync object for testAddRemoveElems

};


#endif //CPISYNCLIB_GENSYNCTESTS_H
