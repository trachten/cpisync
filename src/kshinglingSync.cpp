//
// Created by Bowen Song on 9/23/18.
//

#include <Exceptions.h>
#include "kshinglingSync.h"

kshinglingSync::kshinglingSync(GenSync::SyncProtocol set_sync_protocol, const size_t shingle_size,
        const char stop_word) : myKshingle(shingle_size, stop_word), setSyncProtocol(set_sync_protocol), shingleSize(shingle_size) {
    oneway = true;
    auto setProto_avil = {GenSync::SyncProtocol::IBLTSyncSetDiff, GenSync::SyncProtocol::CPISync,
                          GenSync::SyncProtocol::InteractiveCPISync};
    if (find(setProto_avil.begin(), setProto_avil.end(), set_sync_protocol) == setProto_avil.end())
        throw invalid_argument("Base Set Reconciliation Protocol not supported.");
}

kshinglingSync::~kshinglingSync() {
    for (DataObject* dop : setPointers) delete dop;
}

//Alice
bool kshinglingSync::SyncClient(const shared_ptr<Communicant> &commSync, shared_ptr<SyncMethod> & setHost) {
    Logger::gLog(Logger::METHOD, "Entering kshinglingSync::SyncClient");
    bool syncSuccess = true;

    // call parent method for bookkeeping
    SyncMethod::SyncClient(commSync, setHost);
    // create kshingle

    // connect to server
    commSync->commConnect();
    // ensure that the kshingle size and stopword equal those of the server
    if (!commSync->establishKshingleSend(myKshingle.getElemSize(), myKshingle.getStopWord(), oneway)) {
        Logger::gLog(Logger::METHOD_DETAILS,
                     "Kshingle parameters do not match up between client and server!");
        syncSuccess = false;
    }

    // send cycNum
    if (!oneway){
        if (cycleNum ==0) throw invalid_argument("cycleNum Not prepared by this host, consider enable backtracking before reconciling strings");
        commSync->commSend(cycleNum);
    }
    cycleNum = commSync->commRecv_long();


    // estimate difference
    if (needEst()) {
        StrataEst est = StrataEst(myKshingle.getElemSize());

        for (auto item : myKshingle.getShingleSet_str()) {
            est.insert(new DataObject(item)); // Add to estimator
        }

        // since Kshingling are the same, Strata Est parameters would also be the same.
        commSync->commSend(est.getStrata(), false);

        mbar = commSync->commRecv_long(); // cast long to long long

    }

    // reconcile difference + delete extra
    configurate(setHost, myKshingle.getSetSize());
    for (DataObject* dop : setPointers){
        setHost->addElem(dop); // Add to GenSync
    }
    // choose to send if not oneway (default is one way)
//

    return syncSuccess;
}

//Bob
bool kshinglingSync::SyncServer(const shared_ptr<Communicant> &commSync,  shared_ptr<SyncMethod> & setHost) {
    Logger::gLog(Logger::METHOD, "Entering kshinglingSync::SyncServer");
    bool syncSuccess = true;

    SyncMethod::SyncServer(commSync, setHost);

    commSync->commListen();
    if (!commSync->establishKshingleRecv(myKshingle.getElemSize(), myKshingle.getStopWord(), oneway)) {
        Logger::gLog(Logger::METHOD_DETAILS,
                     "Kshingle parameters do not match up between client and server!");
        syncSuccess = false;
    }

    // send cycNum
    auto tmpcycleNum = cycleNum;
    if (!oneway) cycleNum = commSync->commRecv_long();
     commSync->commSend(tmpcycleNum);

    // estimate difference
    if (needEst()) {
        StrataEst est = StrataEst(myKshingle.getElemSize());

        for (auto item : myKshingle.getShingleSet_str()) {
            est.insert(new DataObject(item)); // Add to estimator
        }

        // since Kshingling are the same, Strata Est parameters would also be the same.
        auto theirStarata = commSync->commRecv_Strata();
        mbar = (est -= theirStarata).estimate();
//        mbar = mbar + mbar / 2; // get an upper bound
        commSync->commSend(mbar); // Dangerous cast

    }

    // reconcile difference + delete extra
    configurate(setHost, myKshingle.getSetSize());
    for (DataObject* dop : setPointers){
        setHost->addElem(dop); // Add to GenSync
    }
    return syncSuccess;
}

void kshinglingSync::configurate(shared_ptr<SyncMethod>& setHost, idx_t set_size) {

    int err = 8;// negative log of acceptable error probability for probabilistic syncs

    if (setSyncProtocol == GenSync::SyncProtocol::CPISync) {
        eltSize = 14 + (myKshingle.getshinglelen_str()+3) * 8; // using k and hardcode to +3 for 2 digits and one ":", finding the max might cuase parameter missmatch
        setHost = make_shared<ProbCPISync>(5e3, eltSize, err, true);
    } else if (setSyncProtocol == GenSync::SyncProtocol::InteractiveCPISync) {
        eltSize = 14 + (myKshingle.getshinglelen_str()+3) * 8;
        setHost = make_shared<InterCPISync>(5, eltSize, err, 3, true);
        //(ceil(log(set_size))>1)?:2;
    } else if (setSyncProtocol == GenSync::SyncProtocol::IBLTSyncSetDiff) {
        (mbar == 0) ? mbar = 10 : mbar;
        eltSize = myKshingle.getshinglelen_str();
        setHost = make_shared<IBLTSync_SetDiff>(mbar, eltSize, true);
    }
}

bool kshinglingSync::reconstructString(DataObject* & recovered_string, const list<DataObject *> & Elems) {
    if (cycleNum != 0)
        myKshingle.clear_shingleSet();

        for (auto elem: Elems) {
            //change here - send pair
            myKshingle.updateShingleSet_str(ZZtoStr(elem->to_ZZ()));
        }
        recovered_string = new DataObject(myKshingle.reconstructStringBacktracking(cycleNum).first);
    return cycleNum != 0;
}

bool kshinglingSync::addStr(DataObject* str, vector<DataObject*> &datum,  bool backtrack){
    // call parent add
    SyncMethod::addStr(str, datum, backtrack);
    strLen = (str->to_string()).size();
    myKshingle.clear_shingleSet();
    myKshingle.inject(str->to_string());
    (backtrack)? cycleNum = myKshingle.reconstructStringBacktracking().second : cycleNum = 0;

    for (DataObject* dop : setPointers)
        delete dop; //Clear SetPointers if any
//    setPointers.clear();
    for (auto item : myKshingle.getShingleSet_str()){
        setPointers.push_back(new DataObject(StrtoZZ(item)));
    }
    datum = setPointers;
    return (!backtrack or cycleNum > 0);
}

long kshinglingSync::getVirMem(){
    return myKshingle.getUsedVM();
}


string kshinglingSync::getName(){ return "This is a kshinglingSync of string reconciliation";}
