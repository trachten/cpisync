/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.cpp
 * Author: kaets
 * 
 * Created on June 27, 2018, 9:49 AM
 */

#include <climits>
#include "FullSyncTest.h"
#include "FullSync.h"
#include "CommSocket.h"
#include "ForkHandle.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FullSyncTest);

FullSyncTest::FullSyncTest() {
}

FullSyncTest::~FullSyncTest() {
}

void FullSyncTest::setUp() {
    const int SEED = 07;
    srand(SEED);
}

void FullSyncTest::tearDown() {
}

DataObject* randDO() {
    return new DataObject(ZZ((((long) rand()) << CHAR_BIT) | rand()));
}

void FullSyncTest::basicTest() {
    // setup DataObjects
    const vector<string> testStrings = {"abc", "def", "ghi"};
    vector<DataObject *> objectsPtr;
    for (auto str : testStrings) {
        objectsPtr.push_back(new DataObject(str));
    }

    // Build server
    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::FullSync).
            setComm(GenSync::SyncComm::socket).
            build();
    // ... add all data objects to the server
    for (auto objPtr : objectsPtr) {
        GenSyncServer.addElem(objPtr);
    }

    // Build client
    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::FullSync).
            setComm(GenSync::SyncComm::socket).
            build();
    // ... add all data object except the first to the client
    bool first = true;
    for (auto objPtr : objectsPtr) {
        if (first) {
            first = false;
            continue;
        }
        GenSyncClient.addElem(objPtr);
    }

    forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
    forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
    // check that the result is correct
    // ... statistics are reasonable
    CPPUNIT_ASSERT(resultServer.bytes > 0);
    CPPUNIT_ASSERT(resultClient.bytes > 0);
    //CPPUNIT_ASSERT(resultServer.CPUtime > 0); // CPUtime could be 0 if this test is fast enough
    //CPPUNIT_ASSERT(resultClient.CPUtime > 0);
    //CPPUNIT_ASSERT(resultServer.totalTime > 0);
    //CPPUNIT_ASSERT(resultClient.totalTime > 0);
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

    CPPUNIT_ASSERT(serverElemsStr.size() > 0);
    CPPUNIT_ASSERT(serverElemsStr.size() == clientElemsStr.size());
    CPPUNIT_ASSERT(multisetDiff(serverElemsStr, clientElemsStr).size() == 0);
    CPPUNIT_ASSERT(multisetDiff(clientElemsStr, serverElemsStr).size() == 0);
}

void FullSyncTest::thoroughTest() {
//    const int TIMES = 100;
//    for(int ii = 0; ii < TIMES; ii++) {
//        vector<DataObject *> objectsPtr;
//
//        GenSync GenSyncServer = GenSync::Builder().
//                setProtocol(GenSync::SyncProtocol::FullSync).
//                setComm(GenSync::SyncComm::socket).
//                build();
//        GenSync GenSyncClient = GenSync::Builder().
//                setProtocol(GenSync::SyncProtocol::FullSync).
//                setComm(GenSync::SyncComm::socket).
//                build();
//
//        const int SHARED = rand() % 50;
//        const int CLIENT_MINUS_SERVER = rand() % 25;
//        const int SERVER_MINUS_CLIENT = rand() % 25;
//
//        for(int jj=0; jj<SHARED; jj++) {
//            DataObject* obj = randDO();
//            GenSyncClient.addElem(obj);
//            GenSyncServer.addElem(obj);
//            objectsPtr.push_back(obj);
//        }
//
//        for(int jj=0; jj<CLIENT_MINUS_SERVER; jj++) {
//            DataObject* obj = randDO();
//            GenSyncClient.addElem(obj);
//            objectsPtr.push_back(obj);
//        }
//
//        for(int jj=0; jj<SERVER_MINUS_CLIENT; jj++) {
//            DataObject* obj = randDO();
//            GenSyncServer.addElem(obj);
//            objectsPtr.push_back(obj);
//        }
//
//        forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
//        forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
//        // check that the result is correct
//        // ... statistics are reasonable
//        CPPUNIT_ASSERT(resultServer.bytes > 0);
//        CPPUNIT_ASSERT(resultClient.bytes > 0);
//        //CPPUNIT_ASSERT(resultServer.CPUtime > 0); // CPUtime could be 0 if this test is fast enough
//        //CPPUNIT_ASSERT(resultClient.CPUtime > 0);
//        //CPPUNIT_ASSERT(resultServer.totalTime > 0);
//        //CPPUNIT_ASSERT(resultClient.totalTime > 0);
//        CPPUNIT_ASSERT(resultServer.success); // i.e. successful synchronization
//        CPPUNIT_ASSERT(resultClient.success);
//
//        // ... that server and client have all the data
//        auto serverElems = GenSyncServer.dumpElements();
//        auto clientElems = GenSyncClient.dumpElements();
//        multiset<string> serverElemsStr, clientElemsStr; // multisets of string representations of the corresponding DataObjects
//
//        for (auto elem : serverElems) {
//            serverElemsStr.insert(elem->print());
//            cout << "Server:" << elem->print() << endl;
//        }
//        for (auto elem : clientElems) {
//            clientElemsStr.insert(elem->print());
//            cout << "Client:" << elem->print() << endl;
//        }
//
//        CPPUNIT_ASSERT(serverElemsStr.size() > 0);
//        auto e = multisetDiff(clientElemsStr, serverElemsStr);
//        CPPUNIT_ASSERT(multisetDiff(serverElemsStr, clientElemsStr).empty());
//        CPPUNIT_ASSERT(multisetDiff(clientElemsStr, serverElemsStr).empty());
//        CPPUNIT_ASSERT_EQUAL(serverElemsStr.size(), clientElemsStr.size());
//
//    }
    // setup DataObjects
    const unsigned long DIFF = 50; // amt of elements shared between client and server

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
    forkHandleReport resultServer = forkHandle(GenSyncServer, GenSyncClient);
    forkHandleReport resultClient = forkHandle(GenSyncClient, GenSyncServer);
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
}