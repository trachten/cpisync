//
// Created by shubham on 7/2/20.
//

#include <iostream>
#include <CPISync/Syncs/GenSync.h>
//#include "CPISync/Syncs/IBLT.h"
#include "CPISync/Syncs/IBLTMultiset.h"
#include "../tests/TestAuxiliary.h"

using namespace std;

int main() {

    const int SIZE = 100;
    const size_t ITEM_SIZE = sizeof(randZZ());
//    ZZ_p::init(randZZ());
//    int BITS = sizeof(randZZ());
//    int PORT = 8001;
//
//    cout <<"BITS : " << BITS << endl;
//    cout <<"CHAR-BIT : " << CHAR_BIT << endl;
//
//    GenSync genSyncClient = GenSync::Builder().
//            setProtocol(GenSync::SyncProtocol::IBLTSync_Multiset).
//            setComm(GenSync::SyncComm::socket).
//            setBits(BITS).
//            setExpNumElems(UCHAR_MAX*4).
//            setPort(PORT).
//            build();
//
//    GenSync genSyncServer = GenSync::Builder().
//            setProtocol(GenSync::SyncProtocol::IBLTSync_Multiset).
//            setComm(GenSync::SyncComm::socket).
//            setBits(BITS).
//            setExpNumElems(UCHAR_MAX*4).
//            setPort(PORT).
//            build();
//
//
////    genSyncClient.addElem(make_shared<DataObject>('a'));
////    genSyncClient.addElem(make_shared<DataObject>('a'));
////    genSyncClient.addElem(make_shared<DataObject>('a'));
////    genSyncClient.addElem(make_shared<DataObject>('x'));
////    genSyncClient.addElem(make_shared<DataObject>('y'));
//
////    genSyncServer.addElem(make_shared<DataObject>('a'));
////    genSyncServer.addElem(make_shared<DataObject>('b'));
////    genSyncServer.addElem(make_shared<DataObject>('c'));
////    genSyncServer.addElem(make_shared<DataObject>('e'));
////    genSyncServer.addElem(make_shared<DataObject>('d'));
//
//    bool isSyncSuccess = syncTest(genSyncClient, genSyncServer, false, false, false, true, false);
//
////    multiset<string> resultantServer;
////    cout << "resultant server list\n";
////    cout << "resultant server size: " << genSyncServer.dumpElements().size() << endl;
////    for (const auto &elem : genSyncServer.dumpElements()) {
////        resultantServer.insert(elem);
////        cout << "server entries: "  << base64_decode(elem) << endl;
////    }
//
//    multiset<string> resultantClient;
//    cout << "resultant client list\n";
//    cout << "resultant client size: " << genSyncClient.dumpElements().size() << endl;
////    for (const auto &elem : genSyncClient.dumpElements()) {
////        resultantClient.insert(elem);
////        cout << "client entries: "  << base64_decode(elem) << endl;
////    }
//
////    multiset<string> reconciled;
////    reconciled.insert("a");
////    reconciled.insert("b");
////    reconciled.insert("c");
////    reconciled.insert("x");
////    reconciled.insert("y");
////    reconciled.insert("d");
////    reconciled.insert("e");
//
////    if(reconciled == resultantServer) {
////        cout << "equals true\n";
////    } else {
////        cout << "equals false\n";
////    }
//
//    if (!isSyncSuccess) {
//        cout << "IBLT sync unsuccessful\n";
//    } else {
//        cout << "IBLT sync succeeded\n";
//    }


//    IBLT iblt(SIZE, ITEM_SIZE, true);
    IBLTMultiset iblt(SIZE, ITEM_SIZE);

    iblt.insert(strToZZ("a"), strToZZ("a"));
    iblt.insert(strToZZ("a"), strToZZ("a"));
    iblt.insert(strToZZ("a"), strToZZ("a"));
    iblt.insert(strToZZ("b"), strToZZ("b"));
    iblt.insert(strToZZ("b"), strToZZ("b"));
//    iblt.insert(strToZZ("b"), strToZZ("b"));
//    iblt.insert(strToZZ("b"), strToZZ("b"));

//    IBLT iblt2(SIZE, ITEM_SIZE, true);
    IBLTMultiset iblt2(SIZE, ITEM_SIZE);

    iblt2.insert(strToZZ("c"), strToZZ("c"));
    iblt2.insert(strToZZ("d"), strToZZ("d"));
    iblt2.insert(strToZZ("e"), strToZZ("e"));
    iblt2.insert(strToZZ("x"), strToZZ("x"));
    iblt2.insert(strToZZ("y"), strToZZ("y"));

    iblt -= iblt2;

    vector<pair<ZZ, ZZ>> pos, neg;
    bool isSuccess = iblt.listEntries(pos, neg);
    if (!isSuccess) {
        cout << "could not list all entries\n";
    } else {
        cout << "can list entries\n";
    }
    cout << "IBLT entries: \n" << "positive\n";
    int k, v;
    for (const auto &elem: pos) {
        NTL::conv(k, elem.first); NTL::conv(v, elem.second);
        cout << (char) k << ", " << (char) v << endl;
    }
    cout << "\nnegative\n";
    for (const auto &elem: neg) {
        NTL::conv(k, elem.first); NTL::conv(v, elem.second);
        cout << (char) k << ", " << (char) v << endl;
    }

    cout << "listing complete\n";

    return 0;
}
