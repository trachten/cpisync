/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/23/2018.
//

#ifndef CPISYNCLIB_GENSYNCTESTS_H
#define CPISYNCLIB_GENSYNCTESTS_H

#include <string>
#include <cppunit/extensions/HelperMacros.h>

#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/GenSync.h>

class GenSyncTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(GenSyncTest);

    CPPUNIT_TEST(testAddRemoveElems);
    CPPUNIT_TEST(testGetName);
    CPPUNIT_TEST(testBuilder);
    CPPUNIT_TEST(testTwoWaySync);
    CPPUNIT_TEST(testTwoWayProbSync);
    CPPUNIT_TEST(testOneWaySync);
    CPPUNIT_TEST(testOneWayProbSync);
    CPPUNIT_TEST(testAddRemoveSyncMethodAndComm);
    CPPUNIT_TEST(testCounters);
    CPPUNIT_TEST(testPort);

	CPPUNIT_TEST_SUITE_END();
public:
    GenSyncTest();

    ~GenSyncTest() override;
    void setUp() override;
    void tearDown() override;

	/**
	* Test that GenSyncs constructed using the GenSync::Builder helper class create equivalent GenSyncs to those
	* constructed using the standard constructor
	*/
    static void testBuilder();

	/**
	* Test syncing all GenSyncs with two-way SyncMethods
	* i.e all sync methods that are able to update both the client and server
	*/
    static void testTwoWaySync();

	/**
	* Test syncing GenSyncs with two-way SyncMethods that have a chance of only partly reconciling
	 * i.e all sync methods that update the client and server but have a probability of only reconciling some of the missing elements
	*/
    static void testTwoWayProbSync();

	/**
	* Test syncing GenSyncs with one-way SyncMethods (Only OneWayCPISync)
	* i.e one of the parties in the sync does not update its set to match the other
	*/
    static void testOneWaySync();

	/**
	* Test syncing GenSyncs with one-way SyncMethods that have a chance of only partly reconciling (Only OneWayIBLTSync)
	 * i.e Only one of the parties attempts to get missing elements from the other party (and has a probability of
	 * getting only some of those differences
	*/
    static void testOneWayProbSync();

	/**
	* Test that GenSync::getName returns some nonempty string
	*/
    static void testGetName();

	/**
	* Test adding and removing data from GenSync
	*/
    void testAddRemoveElems();

	/**
	* Test adding and removing SyncMethods and Communicants from GenSync
	*/
    static void testAddRemoveSyncMethodAndComm();

	/**
	* Test sync time and bytes-exchanged counters
	*/
    static void testCounters();

	/**
	* Tests getPort
	*/
    static void testPort();

private:
    // constants
    const unsigned int ELTS = 64; // amount of elements to be added to the GenSync object for testAddRemoveElems

};


#endif //CPISYNCLIB_GENSYNCTESTS_H
