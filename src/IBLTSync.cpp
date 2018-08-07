//
// Created by eliez on 8/3/2018.
//

#include <Exceptions.h>
#include "IBLTSync.h"

IBLTSync::IBLTSync(size_t expected, size_t eltSize) : myIBLT(expected, eltSize){}

IBLTSync::~IBLTSync() = default;

bool IBLTSync::SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){
    try {

    } catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    } // might not need the try-catch
}
bool IBLTSync::SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){
    try {

        stringstream msg;
        msg << "FullSync succeeded." << endl;
        msg << "self - other = " << printListOfPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());
    } catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    } // might not need the try-catch
}
bool IBLTSync::addElem(DataObject* datum){}
bool IBLTSync::delElem(DataObject* datum){}
string IBLTSync::getName(){}