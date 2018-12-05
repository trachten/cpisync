//
// Created by Bowen Song on 10/14/18.
//

#include "IBLTSync_SetDiff.h"

IBLTSync_SetDiff::IBLTSync_SetDiff(size_t expected_diff, size_t eltSize, bool keep_alive) : myIBLT((expected_diff>0)?expected_diff:0, eltSize) {
    expNumDiff = expected_diff;
    oneWay = false;
    keepAlive = keep_alive;
}
IBLTSync_SetDiff::~IBLTSync_SetDiff(){

};

bool IBLTSync_SetDiff::SyncClient(const shared_ptr<Communicant> &commSync, list<DataObject *> &selfMinusOther,
                                  list<DataObject *> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync::SyncClient");

    bool success = true;
if(!keepAlive) {
    // call parent method for bookkeeping
    SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);
    // connect to server
    commSync->commConnect();
}
    // ensure that the IBLT size and eltSize equal those of the server
    if(!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), oneWay)) {
        Logger::gLog(Logger::METHOD_DETAILS,
                     "IBLT parameters do not match up between client and server!");
        success = false;
    }
    commSync->commSend(myIBLT, true);

    if(!oneWay) {
        list<DataObject*> newOMS = commSync->commRecv_DataObject_List();
        list<DataObject*> newSMO = commSync->commRecv_DataObject_List();

        otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
        selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());

        stringstream msg;
        msg << "IBLTSync_SetDiff succeeded." << endl;
        msg << "self - other = " << printListOfPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());
    }
    return success;
}

bool IBLTSync_SetDiff::SyncServer(const shared_ptr<Communicant> &commSync, list<DataObject *> &selfMinusOther,
                                  list<DataObject *> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync::SyncServer");

    bool success = true;

    if(!keepAlive) {
        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        commSync->commListen();
    }
    // ensure that the IBLT size and eltSize equal those of the server
    if(!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), oneWay)) {
        Logger::gLog(Logger::METHOD_DETAILS,
                     "IBLT parameters do not match up between client and server!");
        success = false;
    }

    // verified that our size and eltSize == theirs
    IBLT theirs = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());

    // more efficient than - and modifies theirs, which we don't care about
    vector<pair<ZZ, ZZ>> positive, negative;

    if(!(theirs-=myIBLT).listEntries(positive, negative)) {
        Logger::gLog(Logger::METHOD_DETAILS,
                     "Unable to completely reconcile, returning a partial list of differences");
        success = false;
    }

    // store values because they're what we care about
    for(auto pair : positive) {
        otherMinusSelf.push_back(new DataObject(pair.second));
    }

    for(auto pair : negative) {
        selfMinusOther.push_back(new DataObject(pair.first));
    }

    // send the difference
    if(!oneWay) {
        commSync->commSend(selfMinusOther);
        commSync->commSend(otherMinusSelf);
    }

    stringstream msg;
    msg << "IBLTSync_SetDiff " << (success ? "succeeded" : "may not have completely succeeded") << endl;
    msg << "self - other = " << printListOfPtrs(selfMinusOther) << endl;
    msg << "other - self = " << printListOfPtrs(otherMinusSelf) << endl;
    Logger::gLog(Logger::METHOD, msg.str());

    for (auto item : otherMinusSelf) delete item;
    for (auto item : selfMinusOther) delete item;

    return success;
}

bool IBLTSync_SetDiff::addElem(DataObject *datum) {
    SyncMethod::addElem(datum);
    myIBLT.insert(datum->to_ZZ(), datum->to_ZZ());
    return true;
}

bool IBLTSync_SetDiff::delElem(DataObject *datum) {
    SyncMethod::delElem(datum);
    myIBLT.erase(datum->to_ZZ(), datum->to_ZZ());
    return true;
}

string IBLTSync_SetDiff::getName() {return "I am an IBLTSync with the following params:\n*expected number of set diff: "
+ toStr(expNumDiff) + "\n*size of values: " + toStr(myIBLT.eltSize());}