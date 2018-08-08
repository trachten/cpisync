//
// Created by eliez on 8/3/2018.
//

#include <Exceptions.h>
#include "IBLTSync.h"

IBLTSync::IBLTSync(size_t expected, size_t eltSize) : myIBLT(expected, eltSize) {
    oneWay = false;
}

IBLTSync::~IBLTSync() = default;

bool IBLTSync::SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){
    try {
        Logger::gLog(Logger::METHOD, "Entering IBLTSync::SyncClient");

        // call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

        // connect to server
        commSync->commConnect();

        // ensure that the IBLT size and eltSize equal those of the server
        if(!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), oneWay))
            Logger::error_and_quit("IBLT parameters do not match up between client and server!");

        commSync->commSend(myIBLT, true);

        // TODO: finish up SyncClient

        stringstream msg;
        msg << "IBLTSync succeeded." << endl;
        msg << "self - other = " << printListOfPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());
    } catch (SyncFailureException s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    } // might not need the try-catch
}
bool IBLTSync::SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){
    try {
        Logger::gLog(Logger::METHOD, "Entering IBLTSync::SyncServer");

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        commSync->commListen();

        // ensure that the IBLT size and eltSize equal those of the server
        if(!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), oneWay))
            Logger::error_and_quit("IBLT parameters do not match up between client and server!");

        // verified that our size and eltSize == theirs
        IBLT theirs = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());

        // TODO: finish up SyncServer

        stringstream msg;
        msg << "IBLTSync succeeded." << endl;
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