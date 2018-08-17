/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 8/3/2018.
//

#ifndef CPISYNCLIB_IBLTSYNC_H
#define CPISYNCLIB_IBLTSYNC_H

#include "SyncMethod.h"
#include "Auxiliary.h"
#include "IBLT.h"

class IBLTSync : public SyncMethod {
public:
    // Constructors and destructors
    IBLTSync(size_t expected, size_t eltSize);
    ~IBLTSync();

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool addElem(DataObject* datum) override;
    bool delElem(DataObject* datum) override;
    string getName();
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
