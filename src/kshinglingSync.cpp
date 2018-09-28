//
// Created by Bowen Song on 9/23/18.
//

#include <Exceptions.h>
#include "kshinglingSync.h"


kshinglingSync::kshinglingSync(size_t shingle_len,GenSync::SyncProtocol sync_protocol,GenSync::SyncComm sync_comm, long edit_distance, long symbol_size) {
    setSyncProtocol = sync_protocol;
    setSyncComm = sync_comm;
    k = shingle_len;
    mbar = (edit_distance+5)*1.7; // 4 is constant if front or/and back is modified
    bits = symbol_size;//*(k+2);  // ascii symbol size is 8
    oneWay = false; // not necessary
    cycleNum = -1;
}
kshinglingSync::~kshinglingSync() = default;


GenSync kshinglingSync::SyncHost(string str,K_Shingle& host_content) {

    host_content.create(str);
    auto Set = host_content.getShingleSet_str();
    int cyc = host_content.reconstructStringBacktracking().second;

    GenSync host = GenSync::Builder().
            setProtocol(setSyncProtocol).
            setMbar(mbar).
            setNumPartitions(3).
            setComm(setSyncComm).
            setBits(pow(bits, 2)).
            setNumPartitions(3).
            build();

    for (auto tmp : Set) {
        host.addElem(&tmp);
    }
    host.addElem(&cyc);
    return host;
}


forkHandleReport kshinglingSync::SyncNreport(GenSync & server, GenSync & client){
    //client.delComm(0);
   // server.delComm(0);
    auto res= forkHandleServer(server,client);

//    for (int i = 0; i <server.numComm() ; ++i) {
//        server.delComm(i);
//    }
//    for (int i = 0; i <client.numComm() ; ++i) {
//        client.delComm(i);
//    }
//    client.delComm(0);
//    server.delComm(0);
    return res;

}

string kshinglingSync::getString(GenSync host,K_Shingle& host_content){
    auto content = host.dumpElements();
    auto tmpnum = -1;
    host_content.clear_ShingleSet();
    for (auto dop : content){
        tmpnum = dop->to_string().find_last_of(":");
        if(tmpnum<0){
            cycleNum = stoi(dop->to_string());
        }else {
            host_content.append_ShingleSet(dop->to_string());
        }
    }
    if(cycleNum<0){
        return "";
    }
    return host_content.reconstructStringBacktracking(cycleNum).first;
}

string kshinglingSync::getName(){ return "This is a kshinglingSync of string reconciliation";}