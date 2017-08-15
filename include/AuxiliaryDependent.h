/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/* 
 * File:   Tools.h
 * Author: Ari Trachtenberg
 * 
 * Helper functions used throughout the cod, but that may depend on project files.
 * so that they
 *
 * Created on August 11, 2017, 2:49 PM
 */

#ifndef AUXILIARY_DEPENDENT_H
#define AUXILIARY_DEPENDENT_H

#include "GenSync.h"
#include "SyncMethod.h"
#include "Logger.h"

/**
 * Report structure for a forkHandle run
  */
struct forkHandleReport {
    forkHandleReport(): bytes(-1), CPUtime(-1), totalTime(-1), success(false) {}
    long bytes;       // the number of bytes communicated
    double CPUtime;   // the amount of CPU time used
    double totalTime; // total time used
    list<DataObject *> serverElements; // the resulting elements in the server's data structure
    list<DataObject *> clientElements; // the resulting elements in the server's data structure
    bool success;     // true iff the sync completed successfully
};

/**
 * Runs client1 (child process) and client2 (parent process), returning statistics for client2.
 * @param server The GenSync object that plays the role of server in the sync.
 * @param client The GenSync object that plays the role of client in the sync.
 * @return Synchronization statistics as reported by the server.
 */
inline forkHandleReport forkHandle(GenSync server, GenSync client) {   
    int err = 1;
    int chld_state;
    int my_opt = 0;
    forkHandleReport result;
    clock_t start = clock();
    try {
        pid_t pID = fork();
        const int method_num = 0; // For now, only one method_num is being used - id 0
        if (pID == 0) {
            signal(SIGCHLD, SIG_IGN);
            Logger::gLog(Logger::COMM,"created a server process");
            client.listenSync(method_num);
            exit(0);
        } else if (pID < 0) {
            //handle_error("error to fork a child process");
            cout << "throw out err = " << err << endl;
            throw err;
        } else {
            Logger::gLog(Logger::COMM,"created a client process");
            server.startSync(method_num);
            result.totalTime = (double) (clock() - start) / CLOCKS_PER_SEC;
            result.CPUtime = server.getSyncTime(method_num); /// assuming <method_num>'th communicator corresponds to <method_num>'th syncagent
            result.bytes = server.getXmitBytes(method_num) + server.getRecvBytes(method_num);
            result.serverElements = server.dumpElements();
            result.clientElements = client.dumpElements();
            waitpid(pID, &chld_state, my_opt);
            result.success=true;
        }

    } catch (int& err) {
        sleep(1); // why?
        cout << "handle_error caught" << endl;
        result.success=false;
    }
    
    return result;
}


#endif /* AUXILIARY_DEPENDENT_H */

