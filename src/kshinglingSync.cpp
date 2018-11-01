////
//// Created by Bowen Song on 9/23/18.
////
//
//#include <Exceptions.h>
//#include "kshinglingSync.h"
//
//
//kshinglingSync::kshinglingSync(GenSync::SyncProtocol sync_protocol,GenSync::SyncComm sync_comm,
//                               size_t symbol_size , int m_bar, int num_Parts, int num_ExpElem, int port_num) {
//    setSyncProtocol = sync_protocol;
//    setSyncComm = sync_comm;
//    mbar = m_bar; // 4 is constant if front or/and back is modified
//    bits = symbol_size;//*(k+2);  // ascii symbol size is 8
//    cycleNum = -1;
//    numParts = num_Parts;
//    numExpElem = num_ExpElem;
//    portNum = port_num;
//}
//kshinglingSync::~kshinglingSync() = default;
//
//
//GenSync kshinglingSync::SyncHost(string str,K_Shingle& host_content) {
//
//    host_content.create(str);
//    auto Set = host_content.getShingleSet_str();
//    auto cyc = to_string(host_content.reconstructStringBacktracking().second);
//
//    GenSync host = GenSync::Builder().
//            setProtocol(setSyncProtocol).
//            setComm(setSyncComm).
//            setMbar(mbar).
//            setNumPartitions(numParts).
//            setBits(bits).
//            setPort(portNum).
//            setNumExpectedElements(numExpElem).
//            build();
//
//    for (auto tmp : Set) {
//        host.addElem(&tmp);
//    }
//    host.addElem(&cyc);
//    return host;
//}
//
//
//forkHandleReport kshinglingSync::SyncNreport(GenSync &server, GenSync client){
//    return forkHandle(server,client, false); // by default we want to get rid of Extra elements : flag false
//}
//
//string kshinglingSync::getString(GenSync host,K_Shingle& host_content){
//    auto content = host.dumpElements();
//    auto tmpnum = -1;
//    host_content.clear_shingleSet();
//    for (auto dop : content){
//        tmpnum = dop->to_string().find_last_of(":");
//        if(tmpnum<0){
//            cycleNum = stoi(dop->to_string());
//        }else {
//            host_content.append_ShingleSet(dop->to_string());
//        }
//    }
//    if(cycleNum<0){
//        return "";
//    }
//    return host_content.reconstructStringBacktracking(cycleNum).first;
//}
//
//string kshinglingSync::getName(){ return "This is a kshinglingSync of string reconciliation";}