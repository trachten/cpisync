//
// Created by eliez on 8/3/2018.
//

#ifndef CPISYNCLIB_IBLTSYNC_H
#define CPISYNCLIB_IBLTSYNC_H

#include "SyncMethod.h"
#include "Auxiliary.h"
#include "IBLT.h"

class IBLTSync : SyncMethod {
public:
    // Constructors and destructors
    IBLTSync(size_t expected, size_t eltSize);
    ~IBLTSync();

    // Implemented parent class methods
    bool SyncClient(shared_ptr<Communicant> commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);
    bool SyncServer(shared_ptr<Communicant> commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);
    bool addElem(DataObject* datum);
    bool delElem(DataObject* datum);
    string getName();
private:
    IBLT myIBLT;
};


#endif //CPISYNCLIB_IBLTSYNC_H
