/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * The IBLTSync sync method syncs with another IBLTSync by sending an IBLT containing
 * its set. Upon receiving this IBLT, the server performs a subtract operation on both IBLTs
 * and uses the resulting IBLT to calculate the symmetric set difference. These differences
 * are then sent back to the client (in the case of a two-way sync), or not communicated (one-way sync).
 *
 * There is a small probability that most, but not all, of the differences will be uncovered as a result
 * of this sync.
 *
 * Created by Eliezer Pearl on 8/3/2018.
 */
#ifndef CPISYNCLIB_IBLTSYNC_H
#define CPISYNCLIB_IBLTSYNC_H

#include "SyncMethod.h"
#include "Auxiliary.h"
#include "IBLT.h"

class IBLTSync : public SyncMethod {
public:
    /*
     * Constructor.
     * @param expected The expected number of elements being stored
     * @param eltSize The size of elements being stored
     */
    IBLTSync(size_t expected, size_t eltSize);
    ~IBLTSync() override;

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool addElem(DataObject* datum) override;
    bool delElem(DataObject* datum) override;
    string getName() override;
protected:
    // one way flag
    bool oneWay;
private:
    // IBLT instance variable for storing data
    IBLT myIBLT;

    // Instance variable to sore the expected number of elements
    size_t expNumElems;
};


#endif //CPISYNCLIB_IBLTSYNC_H
