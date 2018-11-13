//
// Created by Bowen Song on 9/23/18.
// Use one or all set recon methods
//

#ifndef CPISYNCLIB_KSHINGLINGSYNC_H
#define CPISYNCLIB_KSHINGLINGSYNC_H

#include "GenSync.h"
#include "SyncMethod.h"
#include "Auxiliary.h"
#include "kshingling.h"
#include "StrataEst.h"
#include "IBLTSync_SetDiff.h"

class kshinglingSync : public SyncMethod{
public:
    // Constructors and destructors
    /**
     * Constructor for K-shingling Sync
     * @param edit_distance_bar Edit distance upper bound (used for single round sync)
     * @param bits_symbol Bits per symbol (default is ascii symbols which are 8 bits)
     * @param k shingle size; Using CPI sync, m_bar equals edit_distance_bar*k since each edit_distance_bar gets repeated at most k times
     */
//    kshinglingSync(GenSync::SyncProtocol sync_protocol,GenSync::SyncComm sync_comm,
//                   size_t symbol_size , int m_bar, int num_Parts, int num_ExpElem, int port_num=8001);

    kshinglingSync(GenSync::SyncProtocol set_sync_protocol, const size_t shingle_size,
            const char stop_word = '$');

    ~kshinglingSync() {};

     size_t injectString(string str);


    /**
     *
     * @param commSync
     * @param selfString
     * @param otherString
     * @param Estimate whether estimate using existing estimation protocol, set mbar (maximum difference) if false
     * @return
     */
    bool SyncClient(const shared_ptr<Communicant>& commSync, shared_ptr<SyncMethod> & setHost, DataObject &selfString, DataObject &otherString, bool Estimate=true) override;

    bool SyncServer(const shared_ptr<Communicant>& commSync, shared_ptr<SyncMethod> & setHost, DataObject &selfString, DataObject &otherString, bool Estimate=true) override;

//    GenSync SyncHost(K_Shingle& host_content);
//    forkHandleReport SyncNreport(GenSync& server, GenSync client);
//
//    multiset<pair<string,int>> getShingles(K_Shingle& host_content){
//        multiset<pair<string,int>> res;
//        for(auto item : host_content.getShingleSet())
//            res.insert(item);
//        return res;
//    };

    bool reconstructString(DataObject* recovered_string) override;

    vector<DataObject*> addStr(DataObject* datum) override;

    string reconString(size_t cycNum){
        return myKshingle.reconstructStringBacktracking(cycNum).first;
    };
//    /**
//     * @return print a string that is reconstructed from object's current set of shingles
//     */
//    string printString();

    // Get the name of the sync method
    string getName() override;

    long cycleNum;

protected:
    bool oneway;
private:
    K_Shingle myKshingle;
    string Str;
    //GenSync myhost;

    size_t eltSize; // defined by shingle size
    size_t mbar; // defined by set difference estimator ... not necessarily a global variable

    GenSync::SyncProtocol setSyncProtocol;
//    GenSync::SyncComm setSyncComm;

//    int numParts;
//    int numExpElem;
//    int portNum;
    //K_Shingle host_content;

    /**
     * Configure set recon protocols
     */
    GenSync configurate(idx_t set_size, int port_num=8001, GenSync::SyncComm setSyncComm=GenSync::SyncComm::socket);

    /**
     * assess if estimation is needed for the set reconcialition
     * interCPI sync and Full sync does not need estimation
     * @return
     */
    bool needEst(){
        return setSyncProtocol==GenSync::SyncProtocol::IBLTSync or setSyncProtocol==GenSync::SyncProtocol::CPISync;
    };

};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
