//
// Created by Bowen Song on 9/23/18.
//

#include <Exceptions.h>
#include "GenSync.h"
#include "kshinglingSync.h"

kshinglingSync::kshinglingSync(size_t shingle_len,GenSync::SyncProtocol sync_protocol, long edit_distance, long symbol_size) : myKShingle(shingle_len){
    setSyncProtocol = sync_protocol;
    mBar = edit_distance;
    bits = symbol_size;  // ascii symbol size is 8
    oneWay = false;
    k = shingle_len;
    //cycleNum = -1;
}
kshinglingSync::~kshinglingSync() = default;


bool kshinglingSync::SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) {
    try {
        bool success = true;

        Logger::gLog(Logger::METHOD, "Entering kshinglingSync::SyncClient");

        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);


        GenSync::Builder builder = GenSync::Builder();
        switch (setSyncProtocol) {
            case GenSync::SyncProtocol::OneWayCPISync:
                builder.
                        setBits(pow(bits, 2)).
                        setMbar(mBar);
                break;
            case GenSync::SyncProtocol::OneWayIBLTSync:
                builder.
                        setBits(bits).
                        setNumExpectedElements(myKShingle.get_size());
                break;
            default:
                throw invalid_argument("I don't know how to handle this base protocol in string recon yet");
        }
        return success;
    }catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    }
}
bool kshinglingSync::SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){
    try {
        bool success = true;

        Logger::gLog(Logger::METHOD, "Entering kshinglingSync::SyncClient");

        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);


        GenSync::Builder builder = GenSync::Builder();
        switch (setSyncProtocol) {
            case GenSync::SyncProtocol::OneWayCPISync:
                builder.
                        setBits(pow(bits, 2)).
                        setMbar(mBar);
                break;
            case GenSync::SyncProtocol::OneWayIBLTSync:
                builder.
                        setBits(bits).
                        setNumExpectedElements(myKShingle.get_size());
                break;
            default:
                throw invalid_argument("I don't know how to handle this base protocol in string recon yet");
        }
        return success;
    }catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    }
}

bool kshinglingSync::addElem(DataObject* datum){
    SyncMethod::addElem(datum);
    myKShingle.add(*datum);
    return true;
}

bool kshinglingSync::delElem(DataObject* datum){
    SyncMethod::delElem(datum);
    myKShingle.del(*datum);
    return true;
}

vector<pair<string,int>> kshinglingSync::getShingles(string str){
    myKShingle.create(str);
    cycleNum = myKShingle.reconstructStringBacktracking().second;
    return myKShingle.getShingleSet();
}

string kshinglingSync::getName(){ return "This is a kshinglingSync of string reconciliation";}