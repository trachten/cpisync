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

class kshinglingSync : public SyncMethod {
public:
    // Constructors and destructors
    /**
     * Constructor for K-shingling Sync
     * @param edit_distance_bar Edit distance upper bound (used for single round sync)
     * @param bits_symbol Bits per symbol (default is ascii symbols which are 8 bits)
     * @param k shingle size; Using CPI sync, m_bar equals edit_distance_bar*k since each edit_distance_bar gets repeated at most k times
     */
    kshinglingSync(const GenSync::SyncProtocol sync_protocol, const long edit_distance, const long symbol_size,const size_t shingle_len, const bool one_way = false);

    ~kshinglingSync();

    bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;

    /**
     * Add a string in to a host
     * @param datum string
     * @return add success, which should always be the case unless it contains stop words
     */
    bool addElem(DataObject* datum) override;



//    /**
//     * @return print a string that is reconstructed from object's current set of shingles
//     */
//    string printString();

    // Get the name of the sync method
    string getName() override;

protected:
    bool oneWay;

private:
//    K_Shingle myK_Shingle; //K_Shingle(const string str, const size_t k);
//    int k;  //shingle size
    GenSync::SyncProtocol set_sync_protocol;
    long bits;
    long m_bar;
    size_t k;
};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
