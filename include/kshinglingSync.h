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
#include "ProbCPISync.h"
#include "InterCPISync.h"

class kshinglingSync : public SyncMethod {
public:
    // Constructors and destructors

    /**
     * Constructor for K-shingling Sync
     * @param set_sync_protocol based on a set sync protocal
     * @param shingle_size has to be at least 2, the higher the size the higher the communication and auxilary space complexity, lower the time cost
     * @param stop_word default '$' which is avoid by randAsciiStr()
     */
    kshinglingSync(GenSync::SyncProtocol set_sync_protocol, const size_t shingle_size,
                   const char stop_word = '$');

    ~kshinglingSync() {};

    /**
     *
     * @param commSync
     * @param selfString
     * @param otherString
     * @param Estimate whether estimate using existing estimation protocol, set mbar (maximum difference) if false
     * @return
     */
    bool SyncClient(const shared_ptr<Communicant> &commSync, shared_ptr<SyncMethod> &setHost, DataObject &selfString,
                    DataObject &otherString, bool Estimate = true) override;

    bool SyncServer(const shared_ptr<Communicant> &commSync, shared_ptr<SyncMethod> &setHost, DataObject &selfString,
                    DataObject &otherString, bool Estimate = true) override;

    bool reconstructString(DataObject *&recovered_string, const list<DataObject *> &Elems) override;

    vector<DataObject *> addStr(DataObject *datum) override;

    string reconString(size_t cycNum) {
        return myKshingle.reconstructStringBacktracking(cycNum).first;
    };


    // Get the name of the sync method
    string getName() override;

    /**
     * Get the amount of virtual mem used in bytes
     * @return virtual mem used in bytes
     */
    long getVirMem() override;

    long cycleNum;

protected:
    bool oneway;
private:
    K_Shingle myKshingle;
    string Str;

    size_t shingleSize =0, eltSize = 0; // defined by shingle size
    size_t mbar = 0; // defined by set difference estimator ... not necessarily a global variable

    GenSync::SyncProtocol setSyncProtocol;


    /**
     * Configure set recon protocols
     */
    void configurate(shared_ptr<SyncMethod> &setHost, idx_t set_size);

    /**
     * assess if estimation is needed for the set reconcialition
     * interCPI sync and Full sync does not need estimation
     * @return
     */
    bool needEst() {
        return setSyncProtocol == GenSync::SyncProtocol::IBLTSyncSetDiff or
               setSyncProtocol == GenSync::SyncProtocol::CPISync;
    };

};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
