/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   FullSync.h
 * Author: kaets
 *
 * Created on June 27, 2018, 9:37 AM
 */

#ifndef FULLSYNC_H
#define FULLSYNC_H

#include <NTL/RR.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_pXFactoring.h>
#include <algorithm>
#include "Auxiliary.h"
#include "SyncMethod.h"
#include "Exceptions.h"

// namespaces
using namespace NTL;

/**
 * A class that stores and reconciles collections of data. Unlike CPISync, which
 * efficiently reconciles multi-sets, FullSync simply exchanges entire collections
 * from one peer to another for reconciliation.
 */
class FullSync : public SyncMethod {
public:
    
    // General class constructor
    FullSync(bool keep_alive = false);
    
    // General class destructor
    ~FullSync() override;
  
    
    // parent methods to override
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject *> &selfMinusOther, list<DataObject *> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject *> &selfMinusOther, list<DataObject *> &otherMinusSelf) override;
    bool addElem(DataObject* newDatum) override;
    bool delElem(DataObject* newDatum) override;
    inline string getName() override { return "I am a FullSync object."; }

    /**
     * @return A string representing the elements stored in the FullSync object.
     */
    string printElem();

protected:
    bool keepAlive;
private:
    multiset<DataObject*, cmp<DataObject*>> myData;
};

#endif /* FULLSYNC_H */

