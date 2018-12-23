//
// Created by Bowen Song on 10/14/18.
//

#ifndef CPISYNCLIB_IBLTSYNC_SETDIFF_H
#define CPISYNCLIB_IBLTSYNC_SETDIFF_H

#include "SyncMethod.h"
#include "Auxiliary.h"
#include "IBLT.h"

class IBLTSync_SetDiff : public SyncMethod {
public:
    // Constructors and destructors
    IBLTSync_SetDiff(size_t expected_diff, size_t eltSize, bool keep_alive=false);
    ~IBLTSync_SetDiff();

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) override;
    bool addElem(DataObject* datum) override;
    bool delElem(DataObject* datum) override;
    string getName() override;
protected:
    bool oneWay, keepAlive;
private:
    IBLT myIBLT;
    size_t expNumDiff;

    list<DataObject*> MemCare;
};

#endif //CPISYNCLIB_IBLTSYNC_SETDIFF_H
