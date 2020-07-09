//
// Created by shubham on 6/23/20.
//

#include <iostream>
#include <string>
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Data/DataObject.h>
#include "TestAuxiliary.h"
#include <stdlib.h>
#include <CPISync/Syncs/GenSync.h>

using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[]) {

    ZZ_p::init(randZZ());


    const int SEED = 93;
    srand(SEED);
    const int BITS = sizeof(randZZ());
    const int NUM_EXP_ELEMS = 4 * UCHAR_MAX;
    const int PORT0 = 8001;
//    const int PORT1 = 8002;
    const int PORT2 = 8003;


    // IBLTSync
    GenSync IBLTSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setExpNumElems(NUM_EXP_ELEMS).
            setPort(PORT0).
            build();

    GenSync IBLTSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setExpNumElems(NUM_EXP_ELEMS).
            setPort(PORT0).
            build();



    const int mBar = 2 * UCHAR_MAX;
    const int err = 8;
    // CPISync
    GenSync CPISyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setMbar(mBar).
            setErr(err).
            setHashes(true).
            setPort(PORT2).
            build();

    GenSync CPISyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setMbar(mBar).
            setErr(err).
            setHashes(true).
            setPort(PORT2).
            build();

    // read elements of client and server from file
    // add elements to client and server

    ifstream clientDataFile("client1.txt");
    string temp;
    while (getline(clientDataFile, temp, '\n')) {
        IBLTSyncClient.addElem(make_shared<DataObject>(temp));
//        IBLTSetOfSetsSyncClient.addElem(make_shared<DataObject>(temp));
        CPISyncClient.addElem(make_shared<DataObject>(temp));
    }

    ifstream serverDataFile("server1.txt");
    while (getline(serverDataFile, temp, '\n')) {
        IBLTSyncServer.addElem(make_shared<DataObject>(temp));
//        IBLTSetOfSetsSyncServer.addElem(make_shared<DataObject>(temp));
        CPISyncServer.addElem(make_shared<DataObject>(temp));
    }

    // print basic stats about number of elements
    multiset<string> initialClient;
    for (const auto &elem : IBLTSyncClient.dumpElements()) {
        initialClient.insert(elem);
    }

    multiset<string> initialServer;
    for (const auto &elem : IBLTSyncServer.dumpElements()) {
        initialServer.insert(elem);
    }

    multiset<string> resultantUnion;
    resultantUnion = multisetUnion(initialClient, initialServer);

    cout << "Initial Server size: " << initialServer.size()
         << "\nInitial Client size: " << initialClient.size()
         << "\nExpected Resultant size: " << resultantUnion.size() << "\n\n";


    // run sync for IBLTSync with Multiset
    bool isSuccess = false;
    bool isIBLTSetSuccess = false;
    bool isCPISuccess = false;
    if (strcmp(argv[1], "client") == 0) {
        cout << "IBLTSync listening on port " << PORT0 << "..." << endl;
        isSuccess = IBLTSyncClient.clientSyncBegin(0);
        cout << "IBLT Client stats: \n";
        if (isSuccess) cout << "IBLTsync succeeded." << endl; else cout << "IBLTsync failed." << endl;
        cout << "Resultant client size: " << IBLTSyncClient.dumpElements().size() << endl;
        cout << IBLTSyncClient.printStats(0) << "\n\n";

        cout << "Final IBLT client elements: \n";
        for (const auto& it: IBLTSyncClient.dumpElements()) {
            cout << base64_decode(it) << ", ";
        }
        cout << endl;

//        cout << "CPISync listening on port " << PORT2 << "..." << endl;
//        isCPISuccess = CPISyncClient.clientSyncBegin(0);
//        cout << "CPISync Client stats: \n";
//        if (isCPISuccess) cout << "CPIsync succeeded." << endl; else cout << "CPIsync failed." << endl;
//        cout << "Resultant client size: " << CPISyncClient.dumpElements().size() << endl;
//        cout << CPISyncClient.printStats(0) << "\n\n";
//
//        cout << "Final CPI client elements: \n";
//        for (const auto& it: CPISyncClient.dumpElements()) {
//            cout << it << ", ";
//        }
//        cout << endl;

    } else {
        cout << "IBLTSync connecting on port " << PORT0 << "..." << endl;
        isSuccess = IBLTSyncServer.serverSyncBegin(0);
        cout << "IBLT Server stats: \n";
        if (isSuccess) cout << "sync succeeded." << endl; else cout << "sync failed." << endl;
        cout << "Resultant server size: " << IBLTSyncServer.dumpElements().size() << endl;
        cout << IBLTSyncServer.printStats(0) << "\n\n";

        cout << "Final IBLT server elements: \n";
        for (const auto& it: IBLTSyncServer.dumpElements()) {
            cout << base64_decode(it) << ", ";
        }
        cout << endl;


//        cout << "CPISync connecting on port " << PORT2 << "..." << endl;
//        isCPISuccess = CPISyncServer.serverSyncBegin(0);
//        cout << "CPISync Server stats: \n";
//        if (isCPISuccess) cout << "CPIsync succeeded." << endl; else cout << "CPIsync failed." << endl;
//        cout << "Resultant server size: " << CPISyncServer.dumpElements().size() << endl;
//        cout << CPISyncServer.printStats(0) << "\n\n";
//
//        cout << "Final CPI server elements: \n";
//        for (const auto& it: CPISyncServer.dumpElements()) {
//            cout << it << ", ";
//        }
//        cout << endl;
    }

    return 0;
}

