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
#include "ForkHandle.h"

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

/**
 * Tests whether synchronization through a socket (no data) works
 */
bool SocketSync() {
    DataObject *test = new DataObject(string("test"));

    for (auto theProto = GenSync::SyncProtocol::BEGIN;
            theProto != GenSync::SyncProtocol::END;
            ++theProto) {
        cout << "... synchronizing with sync protocol " << (int) (theProto) << endl;

    GenSync GenSyncServer = GenSync::Builder().
            setMbar(5).
            setBits(20).
            setProtocol(theProto).
        setComm(GenSync::SyncComm::socket).
            build();
    GenSync GenSyncClient = GenSync::Builder().
            setMbar(5).
            setBits(20).
            setProtocol(theProto).
            setComm(GenSync::SyncComm::socket).
            build();
        forkHandleReport result = forkHandle(GenSyncServer, GenSyncClient);
        if (!result.success)
            return false;
    }

    return true;
}

void test2() {
    cout << "cpi_system_test test 2" << endl;
    cout << "%TEST_FAILED% time=0 testname=test2 (cpi_system_test) message=error message sample" << endl;
}

int main(int argc, char** argv) {
    cout << "%SUITE_STARTING% cpi_system_tests - no data syncs" << endl;
    cout << "%SUITE_STARTED%" << endl;

    cout << "%TEST_STARTED% test1 (cpi_system_test)" << endl;
    assert(SocketSync()==true);
    cout << "%TEST_FINISHED% time=0 test1 (cpi_system_test)" << endl;

//    cout << "%TEST_STARTED% test2 (cpi_system_test)\n" << endl;
//    test2();
//    cout << "%TEST_FINISHED% time=0 test2 (cpi_system_test)" << endl;
//
//    cout << "%SUITE_FINISHED% time=0" << endl;

    return (EXIT_SUCCESS);
}

