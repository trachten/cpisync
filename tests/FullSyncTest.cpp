/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSyncTest.cpp
 * Author: kaets
 * 
 * Created on June 27, 2018, 9:49 AM
 */

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
}

void FullSyncTest::tearDown() {
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

    forkHandleReport result = forkHandle(GenSyncServer, GenSyncClient);

    // check that the result is correct
    // ... statistics are reasonable
    CPPUNIT_ASSERT(result.bytes > 0);
    //CPPUNIT_ASSERT(result.CPUtime > 0); // CPUtime could be 0 if this test is fast enough
    //CPPUNIT_ASSERT(result.totalTime > 0);
    CPPUNIT_ASSERT(result.success); // i.e. successful synchronization

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

