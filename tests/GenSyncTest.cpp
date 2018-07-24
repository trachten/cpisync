//
// Created by eliez on 7/23/2018.
//

#include "ForkHandle.h"
#include "GenSyncTest.h"
#include "GenSync.h"

CPPUNIT_TEST_SUITE_REGISTRATION(GenSyncTest);
GenSyncTest::GenSyncTest(){}
GenSyncTest::~GenSyncTest(){}
void GenSyncTest::setUp(){
    const int SEED = 617;
    srand(SEED);
}
void GenSyncTest::tearDown(){}
void GenSyncTest::testSync() {
    // TODO: implement this for different protocols and communicants
    // setup DataObjects
    for(int jj = 0; jj < TIMES; jj++) {
        const unsigned char SIMILAR = rand(); // amt of elems common to both GenSyncs
        const unsigned char CLIENT_MINUS_SERVER = rand(); // amt of elems unique to client
        const unsigned char SERVER_MINUS_CLIENT = rand(); // amt of elems unique to server

        vector<DataObject *> objectsPtr;

        for (unsigned long ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; ii++) {
            objectsPtr.push_back(new DataObject(randZZ()));
        }

        // Build server
        GenSync GenSyncServer = GenSync::Builder().
                setProtocol(GenSync::SyncProtocol::CPISync).
                setMbar(0).
                setBits(63).
                setComm(GenSync::SyncComm::socket).
                build();
        // ... add data objects unique to the server
        for (auto iter = objectsPtr.begin(); iter != objectsPtr.begin() + SERVER_MINUS_CLIENT; iter++) {
            GenSyncServer.addElem(*iter);
        }

        // Build client
        GenSync GenSyncClient = GenSync::Builder().
                setProtocol(GenSync::SyncProtocol::CPISync).
                setMbar(0).
                setBits(63).
                setComm(GenSync::SyncComm::socket).
                build();
        // ... add data objects unique to the client
        for (auto iter = objectsPtr.begin() + SERVER_MINUS_CLIENT; iter != objectsPtr.begin() + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; iter++) {
            GenSyncClient.addElem(*iter);
        }

        // add common data objects to both
        for(auto iter = objectsPtr.begin() + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; iter != objectsPtr.end(); iter++) {
            GenSyncClient.addElem(*iter);
            GenSyncServer.addElem(*iter);
        }

        // create the expected reconciled multiset
        multiset<string> reconciled;
        for (auto dop : objectsPtr) {
            reconciled.insert(dop->print());
        }

        // create two processes to test successful reconciliation
        int err = 1;
        int chld_state;
        int my_opt = 0;
        pid_t pID = fork();
        if (pID == 0) {
            signal(SIGCHLD, SIG_IGN);
            forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
            // check reasonable statistics
            CPPUNIT_ASSERT(resultClient.bytes > 0);
            CPPUNIT_ASSERT(resultClient.success);

            multiset<string> resClient;
            for (auto dop : GenSyncClient.dumpElements()) {
                resClient.insert(dop->print());
            }

            // check that expected and resultant reconciled sets match up in both size and contents
            CPPUNIT_ASSERT_EQUAL(reconciled.size(), resClient.size());
            CPPUNIT_ASSERT(multisetDiff(reconciled, resClient).empty());
            exit(0);
        } else if (pID < 0) {
            cout << "throw out err = " << err << endl;
            throw err;
        } else {
            waitpid(pID, &chld_state, my_opt);
            forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
            // check reasonable statistics
            CPPUNIT_ASSERT(resultServer.bytes > 0);
            CPPUNIT_ASSERT(resultServer.success);

            multiset<string> resServer;
            for (auto dop : GenSyncServer.dumpElements()) {
                resServer.insert(dop->print());
            }

            // check that expected and resultant reconciled sets match up in both size and contents
            CPPUNIT_ASSERT_EQUAL(reconciled.size(), resServer.size());
            CPPUNIT_ASSERT(multisetDiff(reconciled, resServer).empty());
        }
    }
}