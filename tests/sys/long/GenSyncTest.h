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
    void tearDown();

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
private:
    // constants
    const int TIMES = 1; // Times to run oneWay and twoWay sync tests

    const size_t eltSize = sizeof(randZZ()) * CHAR_BIT; // size of elements stored in sync tests
    const int mBar = UCHAR_MAX*2; // max differences between client and server in sync tests
    const string iostr = ""; // initial string used to construct CommString
    const bool b64 = true; // whether CommString should communicate in b64
    const string host = "localhost"; // host for CommSocket
    const unsigned int port = 8031; // port for CommSocket
    const int err = 8; // negative log of acceptable error probability for probabilistic syncs
    const int numParts = 3; // partitions per level for divide-and-conquer syncs

    // helpers

    /**
     * returns all possible gensync configurations constructed using GenSync::Builder
     * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
     */
    vector<GenSync> builderCombos();

    /**
     * return a vector of GenSyncs constructed using either of the two GenSync constructors
     * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
     * @param useFile if true, GenSyncs constructed using file-based constructor. otherwise, constructed using other constructor
     */
    vector<GenSync> constructorCombos(bool useFile);

    /**
     * returns gensync configurations that communicate one-way constructed using the standard way of creating GenSync objects (without a file)
     * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
     */
    vector<GenSync> oneWayCombos();

    /**
     * returns gensync configurations that communicate two ways constructed using the standard way of creating GenSync objects (without a file)
     * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
     */
    vector<GenSync> twoWayCombos();

    /**
     * returns all possible gensync configurations constructed using the standard way of creating GenSync objects (without a file)
     * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
     */
    vector<GenSync> standardCombos();

    /**
     * returns all possible gensync configurations constructed by creating GenSync objects using a file to store contents
     * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
     */
    vector<GenSync> fileCombos();

    /**
     * Runs tests assuring that two GenSync objects successfully sync via two-way communication
     * @param oneWay true iff the sync will be one way (only server is reconciled)
     * @param GenSyncServer Server GenSync
     * @param GenSyncClient Client GenSync
     */
    void _syncTest(bool oneWay, GenSync GenSyncServer, GenSync GenSyncClient);

    /**
     * One way synctest
     * @param GenSyncClient
     * @param GenSyncServer
     */
    void syncTestOneWay(GenSync GenSyncClient, GenSync GenSyncServer);

    /**
     * Two way synctest
     * @param GenSyncServer Server GenSync
     * @param GenSyncClient Client GenSync
     */
    void syncTest(GenSync GenSyncClient, GenSync GenSyncServer);

};


#endif //CPISYNCLIB_GENSYNCTEST_H
