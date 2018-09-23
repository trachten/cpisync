//
// Created by Bowen Song on 9/23/18.
//

#ifndef CPISYNCLIB_KSHINGLINGSYNC_H
#define CPISYNCLIB_KSHINGLINGSYNC_H

#include "SyncMethod.h"
#include "Auxiliary.h"
#include "kshingling.h"

class kshinglingSync : public SyncMethod {
public:
    // Constructors and destructors
    /**
     * Constructor for K-shingling Sync
     * @param edit_distance_bar Edit distance upper bound (used for single round sync)
     * @param bits_symbol Bits per symbol (ascii symbols are 8 bits)
     * @param k shingle size
     */
    kshinglingSync(int edit_distance_bar, long bits_symbol, size_t k);
    ~kshinglingSync();

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;


    // Get the name of the sync method
    string getName() override;
private:
    K_Shingle myK_Shingle;
    int k;  //shingle size
};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
