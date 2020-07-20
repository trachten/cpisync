//
// TryMe.cpp - a really simple example of how to use this library.
// Created by eliez on 8/17/2018.
//


#include <iostream>
#include <CPISync/Syncs/GenSync.h>

using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[]) {
    if(strcmp(argv[1], "client")!=0 && strcmp(argv[1], "server")!=0) {
        cout << "usage: 'TryMe client <sync type>' for client mode, 'TryMe server <sync type>' for server mode." << endl;
        cout << "run the client in one terminal instance and the server in another." << endl;
        exit(0);
    }

    GenSync::SyncProtocol prot;
    string type = string(argv[2]);

    // no string switch statements :(
    if(type == "CPISync") {
        prot = GenSync::SyncProtocol::CPISync;
    } else if (type == "InterCPISync") {
        prot = GenSync::SyncProtocol::InteractiveCPISync;
    } else if (type == "OneWayCPISync") {
        prot = GenSync::SyncProtocol::OneWayCPISync;
    } else if (type == "FullSync") {
        prot = GenSync::SyncProtocol::FullSync;
    } else if (type == "IBLTSync") {
        prot = GenSync::SyncProtocol::IBLTSync;
    } else if (type == "OneWayIBLTSync") {
        prot = GenSync::SyncProtocol::OneWayIBLTSync;
    } else if (type == "IBLTSyncMultiset") {
        prot = GenSync::SyncProtocol::IBLTSync_Multiset;
    } else {
        cout << "invalid sync type!" << endl;
        exit(1);
    }

    const int PORT = 8001; // port on which to connect
    const int ERR = 8; // inverse log of error chance
    const int M_BAR = 1; // max differences between server and client
    const int BITS = CHAR_BIT; // bits per entry
    const int PARTS = 3; // partitions per level for partition-syncs
    const int EXP_ELTS = UCHAR_MAX*4; // expected number of elements per set

    GenSync genSync = GenSync::Builder().
            setProtocol(prot).
            setComm(GenSync::SyncComm::socket).
            setPort(PORT).
            setErr(ERR).
            setMbar(M_BAR).
            setBits((prot == GenSync::SyncProtocol::IBLTSync || prot == GenSync::SyncProtocol::OneWayIBLTSync || prot == GenSync::SyncProtocol::IBLTSync_Multiset ? BITS : BITS * CHAR_BIT)).
            setNumPartitions(PARTS).
            setExpNumElems(EXP_ELTS).
            build();

    genSync.addElem(make_shared<DataObject>('a'));
    genSync.addElem(make_shared<DataObject>('b'));
    genSync.addElem(make_shared<DataObject>('c'));

    if(strcmp(argv[1], "client")==0) {
        genSync.addElem(make_shared<DataObject>('x'));
        genSync.addElem(make_shared<DataObject>('y'));
        genSync.addElem(make_shared<DataObject>('c'));
        genSync.addElem(make_shared<DataObject>('c'));


        cout << "listening on port " << PORT << "..." << endl;

        cout << "client elements initial\n";
        for (auto elem: genSync.dumpElements()) {
            cout << base64_decode(elem) << endl;
        }
        cout << "client elements initial end\n";


        bool isClientSucceded = genSync.clientSyncBegin(0);
        if(isClientSucceded) cout << "client sync succeeded." << endl;
        else cout << "client sync failed." << endl;

        cout << "client elements final\n";
        for (auto elem: genSync.dumpElements()) {
            cout << base64_decode(elem) << endl;
        }
        cout << "client elements final end\n";


    } else {
        genSync.addElem(make_shared<DataObject>('e'));
        genSync.addElem(make_shared<DataObject>('d'));
        genSync.addElem(make_shared<DataObject>('b'));
        genSync.addElem(make_shared<DataObject>('b'));
        genSync.addElem(make_shared<DataObject>('b'));

        cout << "connecting on port " << PORT << "..." << endl;

        cout << "server elements initial\n";
        for (auto elem: genSync.dumpElements()) {
            cout << base64_decode(elem) << endl;
        }
        cout << "server elements initial end\n";


        bool isServerSucceded = genSync.serverSyncBegin(0);
        if(isServerSucceded) cout << "server sync succeeded." << endl;
        else cout << "server sync failed." << endl;

        cout << "server elements final\n";
        for (auto elem: genSync.dumpElements()) {
            cout << base64_decode(elem) << endl;
        }
        cout << "server elements final end\n";
    }
}
