//
// Created by eliez on 7/23/2018.
//

#include "ForkHandle.h"
#include "GenSyncTest.h"
#include "GenSync.h"

CPPUNIT_TEST_SUITE_REGISTRATION(GenSyncTest);
GenSyncTest::GenSyncTest(){}
GenSyncTest::~GenSyncTest(){}
void GenSyncTest::setUp(){}
void GenSyncTest::tearDown(){}
void GenSyncTest::testSync() {
    // setup DataObjects
    const unsigned long DIFF = 50; // each set should have totally different sets, each containing 50 elts

    vector<DataObject *> objectsPtr;

    for (unsigned long ii = 0; ii < DIFF * 2; ii++) {
        objectsPtr.push_back(new DataObject(randZZ()));
    }

    // Build server
    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setMbar(110).
            setBits(63).
            setComm(GenSync::SyncComm::socket).
            build();
    // ... add data objects unique to the server
    for (auto iter = objectsPtr.begin(); iter != objectsPtr.begin() + DIFF; iter++) {
        GenSyncServer.addElem(*iter);
    }

    // Build client
    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setMbar(110).
            setBits(63).
            setComm(GenSync::SyncComm::socket).
            build();
    // ... add data objects unique to the client
    for (auto iter = objectsPtr.begin() + DIFF; iter != objectsPtr.end(); iter++) {
        GenSyncClient.addElem(*iter);
    }

    // create the expected reconciled multiset
    multiset<string> reconciled;
    for(auto dop : objectsPtr) {
        reconciled.insert(dop->print());
    }

    int err = 1;
    int chld_state;
    int my_opt = 0;
    pid_t pID = fork();
    int method_num = 0;
    if(pID == 0) {
        signal(SIGCHLD, SIG_IGN);
        forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
        // check reasonable statistics
        CPPUNIT_ASSERT(resultClient.bytes > 0);
        CPPUNIT_ASSERT(resultClient.success);

        multiset<string> resClient;
        for(auto dop : GenSyncServer.dumpElements()) {
            resClient.insert(dop->print());
        }
        CPPUNIT_ASSERT_EQUAL(reconciled.size(), resClient.size());
        CPPUNIT_ASSERT(multisetDiff(reconciled, resClient).empty());
        exit(0);
    } else if (pID < 0) {
        cout << "throw out err = " << err << endl;
        throw err;
    } else {
        forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
        // check reasonable statistics
        CPPUNIT_ASSERT(resultServer.bytes > 0);
        CPPUNIT_ASSERT(resultServer.success);

        multiset<string> resServer;
        for(auto dop : GenSyncServer.dumpElements()) {
            resServer.insert(dop->print());
        }

        CPPUNIT_ASSERT_EQUAL(reconciled.size(), resServer.size());
        CPPUNIT_ASSERT(multisetDiff(reconciled, resServer).empty());
        waitpid(pID, &chld_state, my_opt);
    }



    // create separate processes for the server and client
   // forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
//    forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
//
    // check that the result is correct
    // ... statistics are reasonable
  // CPPUNIT_ASSERT(resultServer.bytes > 0);
    //CPPUNIT_ASSERT(resultClient.bytes > 0);
    //CPPUNIT_ASSERT(result.CPUtime > 0); // CPUtime could be 0 if this test is fast enough
    //CPPUNIT_ASSERT(result.totalTime > 0);
  //  CPPUNIT_ASSERT(resultServer.success); // i.e. successful synchronization
    //CPPUNIT_ASSERT(resultClient.success);

    // ... that server and client have all the data
  //  auto serverElems = GenSyncServer.dumpElements();
  //  auto clientElems = GenSyncClient.dumpElements();
  //  multiset<string> serverElemsStr, clientElemsStr; // multisets of string representations of the corresponding DataObjects

  //  for (auto elem : serverElems) {
 //       serverElemsStr.insert(elem->print());
 //       cout << "Server:" << elem->print() << endl;
 //   }
  //  for (auto elem : clientElems) {
  //      clientElemsStr.insert(elem->print());
  //      cout << "Client:" << elem->print() << endl;
 //   }



}

/*
 *     // setup DataObjects
    const unsigned long DIFF = 50; // each set should have totally different sets, each containing 50 elts

    vector<DataObject *> objectsPtr;
    for (unsigned long ii = 0; ii < DIFF * 2; ii++) {
        objectsPtr.push_back(randDO());
    }

    // Build server
    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::FullSync).
            setComm(GenSync::SyncComm::socket).
            build();
    // ... add data objects unique to the server
    for (auto iter = objectsPtr.begin(); iter != objectsPtr.begin() + DIFF; iter++) {
        GenSyncServer.addElem(*iter);
    }

    // Build client
    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::FullSync).
            setComm(GenSync::SyncComm::socket).
            build();
    // ... add data objects unique to the client
    for (auto iter = objectsPtr.begin() + DIFF; iter != objectsPtr.end(); iter++) {
        GenSyncClient.addElem(*iter);
    }

    // create separate processes for the server and client
    GenSync copy(GenSyncServer);
    forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
    forkHandleReport resultClient = forkHandle(GenSyncClient, copy);

    // check that the result is correct
    // ... statistics are reasonable
    CPPUNIT_ASSERT(resultServer.bytes > 0);
    CPPUNIT_ASSERT(resultClient.bytes > 0);
    //CPPUNIT_ASSERT(result.CPUtime > 0); // CPUtime could be 0 if this test is fast enough
    //CPPUNIT_ASSERT(result.totalTime > 0);
    CPPUNIT_ASSERT(resultServer.success); // i.e. successful synchronization
    CPPUNIT_ASSERT(resultClient.success);

    // ... that server and client have all the data
    auto serverElems = GenSyncServer.dumpElements();
    auto clientElems = GenSyncClient.dumpElements();
    multiset<string> serverElemsStr, clientElemsStr; // multisets of string representations of the corresponding DataObjects

    for (auto elem : serverElems) {
        serverElemsStr.insert(elem->print());
        cout << "Server:" << elem->print() << endl;
    }
    for (auto elem : clientElems) {
        clientElemsStr.insert(elem->print());
        cout << "Client:" << elem->print() << endl;
    }

    CPPUNIT_ASSERT(!serverElemsStr.empty());
    CPPUNIT_ASSERT_EQUAL(serverElemsStr.size(), clientElemsStr.size());
    CPPUNIT_ASSERT(multisetDiff(serverElemsStr, clientElemsStr).empty());
    CPPUNIT_ASSERT(multisetDiff(clientElemsStr, serverElemsStr).empty());
 */