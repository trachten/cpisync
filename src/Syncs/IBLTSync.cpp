/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 8/3/2018.
//

#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/IBLTSync.h>

IBLTSync::IBLTSync(size_t expected, size_t eltSize, bool _isMultisetSync) : myIBLT(expected, eltSize, _isMultisetSync) {
    expNumElems = expected;
    oneWay = false;
    isMultisetSync = _isMultisetSync;
    cout << "IBLT Sync init with multiset: " << isMultisetSync << endl;
}

IBLTSync::~IBLTSync() = default;

bool IBLTSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf){
    try {

        Logger::gLog(Logger::TEST, "Entering IBLTSync::SyncClient");
//        Logger::gLog(Logger::TEST, "client listing entries");
//        vector<pair<ZZ, ZZ>> pos, neg;
//        IBLT myIBLTCopy(myIBLT);
//        bool isSuccess = myIBLTCopy.listEntries(pos, neg);
//        if (!isSuccess) {
//            cout << "client could not list all entries\n";
//        } else {
//            cout << "client can list entries\n";
//        }
//        cout << "IBLT entries: \n" << "positive\n";
//        for (const auto &elem: pos) {
//            cout << "client: " << elem.first << ", " << elem.second << endl;
//        }
//        cout << "\nnegative\n";
//        for (const auto &elem: neg) {
//            cout << "client: " << elem.first << ", " << elem.second << endl;
//        }
//        cout << "listing complete\n";
//
        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

        // connect to server
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commConnect();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);


        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if(!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), oneWay)) {
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }
        commSync->commSend(myIBLT, true);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);


        if(!oneWay) {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            stringstream msg;
            msg << "[client]IBLTSync succeeded." << endl;
//            msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
//            msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
            msg << "self - other = " << selfMinusOther.size() << endl;
            msg << "other - self = " << otherMinusSelf.size() << endl;
            Logger::gLog(Logger::TEST, msg.str());
        }

		//Record Stats
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return success;
    } catch (SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    } // might not need the try-catch
}

bool IBLTSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf){
    try {
        Logger::gLog(Logger::TEST, "Entering IBLTSync::SyncServer");
//        Logger::gLog(Logger::TEST, "server listing entries");
//
//        vector<pair<ZZ, ZZ>> pos, neg;
//        IBLT myIBLTCopy(myIBLT);
//        bool isSuccess = myIBLTCopy.listEntries(pos, neg);
//        if (!isSuccess) {
//            cout << "server could not list all entries\n";
//        } else {
//            cout << "server can list entries\n";
//        }
//        cout << "IBLT entries: \n" << "positive\n";
//        for (const auto &elem: pos) {
//            cout << "server: " << elem.first << ", " << elem.second << endl;
//        }
//        cout << "\nnegative\n";
//        for (const auto &elem: neg) {
//            cout << "server: " << elem.first << ", " << elem.second << endl;
//        }
//        cout << "listing complete\n";

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commListen();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        if(!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), oneWay)) {
            Logger::gLog(Logger::TEST, "IBLT parameters do not match up between client and server!");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }

        // verified that our size and eltSize == theirs
        IBLT theirs = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize(), isMultisetSync);

        // print their IBLT

        cout << "Printing their IBLT\n";
        cout << "their IBLT size: " << theirs.size() << endl;
        cout << "My IBLT size: " << myIBLT.size() << endl;
//        IBLT theirCopy(theirs);
//        vector<pair<ZZ, ZZ>> pos, neg;
//        bool isSuccess = theirCopy.listEntries(pos, neg);
//        if (!isSuccess) {
//            cout << "could not list all entries\n";
//        } else {
//            cout << "can list entries\n";
//        }
//        cout << "IBLT entries: \n" << "positive\n";
//        int k, v;
//        for (const auto &elem: pos) {
//            NTL::conv(k, elem.first);
//            NTL::conv(v, elem.second);
//            cout << (char) k << ", " << (char) v << endl;
//        }
//        cout << "\nnegative\n";
//        for (const auto &elem: neg) {
//            NTL::conv(k, elem.first);
//            NTL::conv(v, elem.second);
//            cout << (char) k << ", " << (char) v << endl;
//        }
//
//        cout << "listing complete\n";

        // stop print




        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        Logger::gLog(Logger::TEST, "server received their IBLT");

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        // more efficient than - and modifies theirs, which we don't care about
        vector<pair<ZZ, ZZ>> positive, negative;
        if(!(theirs -= myIBLT).listEntries(positive, negative)) {
            Logger::gLog(Logger::TEST,
                         "Unable to completely reconcile, returning a partial list of differences");
            success = false;
        }

        Logger::gLog(Logger::TEST, "server listing complete");

        // store values because they're what we care about
        for(const auto& pair : positive) {
            otherMinusSelf.push_back(make_shared<DataObject>(pair.second));
        }

        for(const auto& pair : negative) {
            selfMinusOther.push_back(make_shared<DataObject>(pair.first));
        }
        mySyncStats.timerEnd(SyncStats::COMP_TIME);


        if(!oneWay) {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(selfMinusOther);
            commSync->commSend(otherMinusSelf);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
        }

        ofstream outFile("outServerStats.txt");
        outFile << "selfMinusOther\n";
        for (auto it: selfMinusOther) {
            outFile << it->print() << endl;
        }
        outFile << "\notherMinusSelf\n";
        for (auto it: otherMinusSelf) {
            outFile << it->print() << endl;
        }


        stringstream msg;
        msg << "[server]IBLTSync " << (success ? "succeeded" : "may not have completely succeeded") << endl;
//        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
//        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
        msg << "self - other = " << selfMinusOther.size() << endl;
        msg << "other - self = " << otherMinusSelf.size() << endl;
        Logger::gLog(Logger::TEST, msg.str());

		//Record Stats
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return success;
    } catch (SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    } // might not need the try-catch
}
bool IBLTSync::addElem(shared_ptr<DataObject> datum){
    // call parent add
    SyncMethod::addElem(datum);
    myIBLT.insert(datum->to_ZZ(), datum->to_ZZ());
    return true;
}
bool IBLTSync::delElem(shared_ptr<DataObject> datum){
    // call parent delete
    SyncMethod::delElem(datum);
    myIBLT.erase(datum->to_ZZ(), datum->to_ZZ());
    return true;
}
string IBLTSync::getName(){ return "IBLTSync\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(myIBLT.eltSize()) + '\n';}