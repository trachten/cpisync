//
// Created by shubham on 6/4/20.
//

#include <iostream>
#include <string>
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Data/DataObject.h>
#include "TestAuxiliary.h"
#include <stdlib.h>

using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[]) {

    int total = 1;
    if (argc == 2) {
        total = atoi(argv[1]);
    }

    GenSync::SyncProtocol prot = GenSync::SyncProtocol::CuckooSync;

    const size_t bits = sizeof(randZZ());
    const size_t fngprtSize = 12;
    const size_t bucketSize = 4;
    const size_t filterSize = 1 << 8;
    const size_t maxKicks = 500;

    GenSync server = GenSync::Builder()
            .setProtocol(GenSync::SyncProtocol::CuckooSync)
            .setComm(GenSync::SyncComm::socket)
            .setBits(bits)
            .setFngprtSize(fngprtSize)
            .setBucketSize(bucketSize)
            .setFilterSize(filterSize)
            .setMaxKicks(maxKicks)
            .build();

    GenSync client = GenSync::Builder()
            .setProtocol(GenSync::SyncProtocol::CuckooSync)
            .setComm(GenSync::SyncComm::socket)
            .setBits(bits)
            .setFngprtSize(fngprtSize)
            .setBucketSize(bucketSize)
            .setFilterSize(filterSize)
            .setMaxKicks(maxKicks)
            .build();

    ZZ_p::init(randZZ());

//    Logger::gLog(Logger::TEST,"CuckooSyncTest, pid: " + toStr(getpid()));

    double successCount = 0, failCount = 0;
    for (int i = 0; i < total; i++) {
        bool isSuccess = syncTest(client, server, false, false, false, false, false);
//        bool isSuccess = forkHandleServer(server, client).success;
        if (isSuccess) {
            successCount += 1;
            cout << "sync succeeded." << endl;
        } else {
            failCount += 1;
            cout << "sync failed." << endl;
        }
    }

    cout << "Number of iterations: " << total << endl;
    cout << "Cuckoo sync success: " << successCount * 100 / total << "%" << endl;
    cout << "Cuckoo sync fail: " << failCount * 100 / total << "%" << endl;

}
