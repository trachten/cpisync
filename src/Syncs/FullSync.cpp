/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSync.cpp
 * Author: Eliezer Pearl
 * 
 * Created on June 27, 2018, 9:37 AM
 */

#include <CPISync/Syncs/FullSync.h>

FullSync::FullSync() = default;

FullSync::~FullSync() = default;

string FullSync::printElem() {
    auto iter = SyncMethod::beginElements();

    stringstream ss;
    ss << '[';
    
    for(;  iter != SyncMethod::endElements(); iter++)
        ss << **iter << (iter == prev(SyncMethod::endElements()) ? ']' : ' ');
    
    return ss.str();

}

bool FullSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf){
    try{
        Logger::gLog(Logger::METHOD, "Entering FullSync::SyncClient");

        // call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        // connect to the other party
        commSync->commConnect();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        // send my set:
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // first send the amount of DataObjects...
        commSync->commSend(SyncMethod::getNumElem());


        // then send each DataObject.
        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            commSync->commSend(**iter);
        }

        // receive response from server with differences
        selfMinusOther = commSync->commRecv_DoList();
        otherMinusSelf = commSync->commRecv_DoList();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        stringstream msg;
        msg << "FullSync succeeded." << endl;
        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

        commSync->commClose();

		//Record Stats
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return true;
    } catch(SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    }
    
}
bool FullSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf){
    try {
        Logger::gLog(Logger::METHOD, "Entering FullSync::SyncServer");

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        // listen for other party
        commSync->commListen();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // first, receive how many DataObjects have been sent...
        const long SIZE = commSync->commRecv_long();

        // then receive each DataObject and store to a multiset
        multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> other;
        for (int ii = 0; ii < SIZE; ii++) {
            other.insert(commSync->commRecv_DataObject());
        }
        mySyncStats.timerEnd(SyncStats::COMM_TIME);


        mySyncStats.timerStart(SyncStats::COMP_TIME);
        // Calculate differences between two lists and splice onto respective lists
        rangeDiff(myData.begin(), myData.end(), other.begin(), other.end(), back_inserter(selfMinusOther));
        rangeDiff(other.begin(), other.end(), myData.begin(), myData.end(), back_inserter(otherMinusSelf));
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // send back differences. our otherMinusSelf is their selfMinusOther and v.v.
        commSync->commSend(otherMinusSelf);
        commSync->commSend(selfMinusOther);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        stringstream msg;
        msg << "FullSync succeeded." << endl;
        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;

        Logger::gLog(Logger::METHOD, msg.str());

        commSync->commClose();

		//Record Stats
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return true;
    } catch (SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    }
    
}

bool FullSync::addElem(shared_ptr<DataObject> newDatum){
    Logger::gLog(Logger::METHOD,"Entering FullSync::addElem");

    if(!SyncMethod::addElem(newDatum)) return false;
    myData.insert(newDatum);
    Logger::gLog(Logger::METHOD, "Successfully added shared_ptr<DataObject> {" + newDatum->print() + "}");
    return true;
    
}

bool FullSync::delElem(shared_ptr<DataObject> newDatum){
    Logger::gLog(Logger::METHOD, "Entering FullSync::delElem");

    if(!SyncMethod::delElem(newDatum)) return false;
    myData.erase(newDatum);
    Logger::gLog(Logger::METHOD, "Successfully removed shared_ptr<DataObject> {" + newDatum->print() + "}");
    return true;
}