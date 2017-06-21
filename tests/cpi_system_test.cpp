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


bool SocketSync() {
    cout << "cpi_system_test test 1" << endl;
    
    // required data structures
    vector<Communicant*> clist;
    vector<SyncMethod*> mlist;
    list<DataObject*> data;
    GenSync *theSync;
    string test="test";
    
    // populate data structures
    clist.push_back(new CommSocket(9999,"localhost"));
    mlist.push_back(new CPISync(5,20,3));
    data.push_back(new DataObject(test));
    theSync = new GenSync(clist, mlist, data);
    
    long bytes=0;
    double CPUtime, totalTime;
    forkHandleReport result = forkHandle(theSync,theSync);
    
    cout << "Bytes transmitted: " << result.bytes << "; CPU time: " << result.CPUtime << "; total time: " << result.totalTime << endl;
    return result.success;
}

void test2() {
    cout << "cpi_system_test test 2" << endl;
    cout << "%TEST_FAILED% time=0 testname=test2 (cpi_system_test) message=error message sample" << endl;
}

int main(int argc, char** argv) {
    cout << "%SUITE_STARTING% cpi_system_test" << endl;
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

