//
// Created by Bowen Song on 9/23/18.
//

#include <Exceptions.h>
#include "GenSync.h"
#include "kshinglingSync.h"

kshinglingSync::kshinglingSync(const GenSync::SyncProtocol sync_protocol, const long edit_distance, const long symbol_size, const size_t shingle_len, const bool one_way){
    set_sync_protocol = sync_protocol;
    m_bar = edit_distance;
    bits = symbol_size;  // ascii symbol size is 8
    oneWay = one_way;
    k = shingle_len;

}
kshinglingSync::~kshinglingSync() = default;

bool kshinglingSync::SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) {
    try {
        Logger::gLog(Logger::METHOD, "Entering kshinglingSync::SyncClient");

        GenSync GenSyncClient = GenSync::Builder().
                setProtocol(GenSync::SyncProtocol::IBLTSync).
                setComm(GenSync::SyncComm::socket).
                setNumPartitions(2). // hard code to 2 partition, change later
                setBits(bits).
                build();

        return true;
    } catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    } // might not need the try-catch
}

bool kshinglingSync::SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) {
    try {
        Logger::gLog(Logger::METHOD, "Entering kshinglingSync::SyncClient");
        GenSync GenSyncServer = GenSync::Builder().
                setProtocol(GenSync::SyncProtocol::IBLTSync).
                setComm(GenSync::SyncComm::socket).
                setNumPartitions(2). // hard code to 2 partition, change later
                setBits(bits).
                build();

        return true;
    } catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    }
}


bool kshinglingSync::addElem(DataObject* datum) {
    auto set = K_Shingle(datum->to_string(),k).getShingleSet();

    for (int i = 0; i < set.size(); ++i) {

    }

}



string kshinglingSync::getName(){ return "This is a kshinglingSync of string reconciliation";}