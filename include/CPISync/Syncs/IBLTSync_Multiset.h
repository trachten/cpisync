//
// Created by shubham on 7/12/20.
//

#ifndef CPISYNC_IBLTSYNC_MULTISET_H
#define CPISYNC_IBLTSYNC_MULTISET_H

#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Syncs/IBLTSync.h>

class IBLTSync_Multiset : public IBLTSync {
public:
    // Duplicate the IBLTSync constructor, but set multiset to true
    IBLTSync_Multiset(size_t expected, size_t eltSize) : IBLTSync(expected, eltSize) {
        isMultiset = true;
        SyncID = SYNC_TYPE::IBLTSync_Multiset;
    }

    string getName() override {return IBLTSync::getName() + "   * multiset\n";}
};

#endif //CPISYNC_IBLTSYNC_MULTISET_H
