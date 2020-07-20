//
// Created by shubham on 7/12/20.
//

#ifndef CPISYNC_IBLTSYNC_MULTISET_H
#define CPISYNC_IBLTSYNC_MULTISET_H

#include <CPISync/Syncs/IBLTMultiset.h>
#include <CPISync/Aux/SyncMethod.h>

class IBLTSync_Multiset : public SyncMethod {
public:
    // Duplicate the IBLTSync constructor, but set multiset to true
    IBLTSync_Multiset(size_t expected, size_t eltSize);

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool addElem(shared_ptr<DataObject> datum) override;
    bool delElem(shared_ptr<DataObject> datum) override;

    string getName() override;

private:

    // one way flag
    bool oneWay;

    // IBLTMultiset instance variable for storing data
    IBLTMultiset myIBLT;

    // Instance variable to sore the expected number of elements
    size_t expNumElems;
};

#endif //CPISYNC_IBLTSYNC_MULTISET_H
