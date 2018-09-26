//
// Created by Bowen Song on 9/23/18.
//

#include <Exceptions.h>
#include "kshinglingSync.h"


kshinglingSync::kshinglingSync(size_t shingle_len,GenSync::SyncProtocol sync_protocol, long edit_distance, long symbol_size){
    setSyncProtocol = sync_protocol;
    mbar = edit_distance+4; // 4 is constant if front or/and back is modified
    bits = symbol_size;  // ascii symbol size is 8
    oneWay = false; // not necessary
    k = shingle_len;
}
kshinglingSync::~kshinglingSync() = default;


GenSync kshinglingSync::SyncHost(string str) {
    auto host_content = K_Shingle(k);
    host_content.create(str);
    auto Set = host_content.getStrShingleSet();


//    switch (setSyncProtocol) {
//        case GenSync::SyncProtocol::CPISync:
            GenSync host = usingCPISync();
            for (auto tmp : Set) {
                host.addElem(&tmp);
            }
            int *tmp;
            int a = host_content.reconstructStringBacktracking().second;
            tmp=&a;
            host.addElem(tmp);
            return host;

//        case SyncProtocol::InteractiveCPISync:

//            break;
//        case SyncProtocol::OneWayCPISync:

//            break;
//        case SyncProtocol::FullSync:

//            break;
//        case SyncProtocol::IBLTSync:

//            break;
//        case SyncProtocol::OneWayIBLTSync:
//
//            break;
//        default:
//            throw invalid_argument("I don't know how to synchronize with this protocol.");
//    }

}


GenSync kshinglingSync::usingCPISync(){
    GenSync host = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setMbar(mbar).
            setComm(GenSync::SyncComm::socket).
            setBits(bits*(k+2)).
            build();
    return host;
}

forkHandleReport kshinglingSync::SyncServer(GenSync & server, GenSync & client){
    return forkHandleServer(server,client);
}

string kshinglingSync::getHostStr(GenSync host){
    auto content = host.dumpElements();
    vector<pair<string,int>> set;
    for (auto dop : content){
        if(dop->RepIsInt){
            cycleNum = stoi(dop->to_string());
        }
        auto tmp = dop->to_string();

        set.push_back(make_pair(tmp.substr(0,k),stoi(tmp.substr(k+1))));
    }
}

string kshinglingSync::getName(){ return "This is a kshinglingSync of string reconciliation";}