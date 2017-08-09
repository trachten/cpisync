/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cpi_system_test.cpp
 * Author: trachten
 *
 * Created on June 15, 2017, 3:56 PM
 */

#include <stdlib.h>
#include <iostream>
#include <list>
#include <thread>
#include <cassert>

#include "Auxiliary.h"
#include "GenSync.h"
#include "Communicant.h"
#include "CommSocket.h"
#include "CPISync.h"
#include "DataObject.h"

/*
 * Simple C++ Test Suite
 */

// some helpers

/**
 * An awkward helper for iterating enums.
 * @param curr The current enum value
 * @return the next enum value
 */
template <typename T>
T &operator++(T& curr) {
    curr = (T)(((int) (curr) + 1));
    return curr;
}

template <typename T>
list<T> deref(list<T*> in) {
    list<T> result;
    result.resize(in.size());
    transform(in.begin(), in.end(), result.begin(), [](T * datum) {
        return *datum; });

    return result;
}

/**
 *  Tests whether synchronization through a socket (no data) works
 * @param theProto The sync protocol to test
 * @param mBar The assumed maximum number of differences between the syncing hosts.
 * @param Bits The number of bits per datum.
 * @param dataList1 A list of data on the first syncing host.
 * @param dataList2 A list of data on the second syncing host.
 * @return true iff the synchronization was successful *and* resulting client data is a subset of the server data (one-way sync check).
 */
bool SocketSync(GenSync::SyncProtocol theProto, int mBar, int Bits, list<DataObject*> dataList1, list<DataObject*> dataList2) {
    Logger::gLog(Logger::METHOD, "... synchronizing with sync protocol " + toStr((int) theProto));

    GenSync GenSyncServer = GenSync::Builder().
            setMbar(mBar).
            setBits(Bits).
            setProtocol(theProto).
            setComm(GenSync::SyncComm::socket).
            setData(dataList1).
            build();
    GenSync GenSyncClient = GenSync::Builder().
            setMbar(mBar).
            setBits(Bits).
            setProtocol(theProto).
            setComm(GenSync::SyncComm::socket).
            setData(dataList2).
            build();
    forkHandleReport result = forkHandle(GenSyncServer, GenSyncClient);
    Logger::gLog(Logger::METHOD, printList(result.clientElements));
    Logger::gLog(Logger::METHOD, printList(result.serverElements));

    if (!result.success || !listSubsetQ(
            deref(result.clientElements),
            deref(result.serverElements)))
        return false;
    return true;
}

void testBasicProtocols() {
    cout << "cpi_system_test testBasicProtocols" << endl;

    // setup
    DataObject *test1 = new DataObject(string("t1"));
    DataObject *test2 = new DataObject(string("t2"));
    list<DataObject*> dataList1, dataList2;
    dataList1.push_back(test1);
    dataList2.push_back(test2);

    for (auto theProto = GenSync::SyncProtocol::BEGIN;
            theProto != GenSync::SyncProtocol::END;
            ++theProto)
        if (!SocketSync(theProto, 5, 25, dataList1, dataList2)) {
            cout << "%TEST_FAILED% time=0 testname=testBasicProtocols (cpi_system_test) message=Synchronization Failed" << endl;
            return;
        }
}

int main(int argc, char** argv) {
    cout << "%SUITE_STARTING% cpi_system_tests - no data syncs" << endl;
    cout << "%SUITE_STARTED%" << endl;

    cout << "%TEST_STARTED% testBasicProtocols (cpi_system_test)" << endl;
    testBasicProtocols();
    cout << "%TEST_FINISHED% time=0 testBasicProtocols (cpi_system_test)" << endl;

//    cout << "%TEST_STARTED% test2 (cpi_system_test)\n" << endl;
//    test2();
//    cout << "%TEST_FINISHED% time=0 test2 (cpi_system_test)" << endl;
//
//    cout << "%SUITE_FINISHED% time=0" << endl;

    return (EXIT_SUCCESS);
}

