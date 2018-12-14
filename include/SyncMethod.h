/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#ifndef SYNC_METHODS_H
#define SYNC_METHODS_H

#include <algorithm>
#include <vector>
#include <memory>
#include "Communicant.h"

// namespaces
using std::vector;
using std::list;

/**
 * SyncMethod.h -- abstract class for sync methods
 * This is the base class for all synchronization methods.
 */

class SyncMethod {

public:
    // CONSTRUCTOR/DESTRUCTOR

    // constructor
    SyncMethod();
    // destructor
    virtual ~SyncMethod();

    // SYNC

    /**
     * Connect as a client to a specific communicant and computes differences between the two (without actually updating them).
     * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference).
     * %R:  Sync_Server must have been called at that communicant.
     * 
     * @param commSync The communicant to whom to connect.
     * @param selfMinusOther A result of reconciliation.  Elements that I have that the other SyncMethod does not.
     * @param otherMinusSlef A result of reconciliation.  Elements that the other SyncMethod has that I do not.
     * @return true iff the connection and subsequent synchronization appear to be successful.
     */
    virtual bool SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) {
        commSync->resetCommCounters();
        return true;
    }

    /**
     * Waits for a client to connect from a specific communicant and computes differences between the two (without actually updating them).
     * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference).
     *      * 
     * @param commSync The communicant to whom to connect.
     * @param selfMinusOther A result of reconciliation.  Elements that I have that the other SyncMethod does not.
     * @param otherMinusSlef A result of reconciliation.  Elements that the other SyncMethod has that I do not.
     * @return true iff the connection and subsequent synchronization appear to be successful.
     */
    virtual bool SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) {
        commSync->resetCommCounters();
        return true;
    }

    // String Sync
    /**
     * Connect as a client to a specific communicant and computes differences between the two (without actually updating them).
     * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference).
     * %R:  Sync_Server must have been called at that communicant.
     *
     * @param commSync The communicant to whom to connect.
     * @param selfMinusOther A result of reconciliation.  Elements that I have that the other SyncMethod does not.
     * @param otherMinusSlef A result of reconciliation.  Elements that the other SyncMethod has that I do not.
     * @return true iff the connection and subsequent synchronization appear to be successful.
     */
    virtual bool SyncClient(const shared_ptr<Communicant>& commSync,shared_ptr<SyncMethod> & setHost) {
        commSync->resetCommCounters();
        return true;
    }

    /**
     * Waits for a client to connect from a specific communicant and computes differences between the two (without actually updating them).
     * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference).
     *      *
     * @param commSync The communicant to whom to connect.
     * @return true iff the connection and subsequent synchronization appear to be successful.
     */
    virtual bool SyncServer(const shared_ptr<Communicant>& commSync, shared_ptr<SyncMethod> & setHost) {
        commSync->resetCommCounters();
        return true;
    }

    // MANIPULATE DATA
    /**
     * Add an element to the data structure that will be performing the synchronization.
     * @param datum The element to add.  Note:  a pointer to this item is added to the
     * hash, so it is advisable not to change the datum dereference hereafter.
     * @return true iff the addition was successful
     */
    virtual bool addElem(DataObject* datum) { elements.push_back(datum); return true; };

    /**
     * Delete an element from the data structure that will be performing the synchronization.
     * @param datum The element to delete.
     * @return true iff the removal was successful
     */
    virtual bool delElem(DataObject* datum) { 
        long int before = elements.size();
        elements.erase(std::remove(elements.begin(), elements.end(), datum), elements.end());
        return before > elements.size(); // true iff there were more elements before removal than after
    };

    /**
     * Update string from the data structure
     * Add a set to Set Recon Method
     * @param str
     * @return
     */
    virtual bool addStr(DataObject* str, vector<DataObject*> &datum,  bool backtrack){
        originStr = str;
        isStringRecon = true;
        return true;
    }

    // INFORMATIONAL
    /**
     * @return A human-readable name for the synchronization method.
     */
    virtual string getName() = 0;

    virtual long getVirMem(){return 0;};

    virtual bool reconstructString(DataObject* & recovered_string, const list<DataObject *> & Elems){
        return true;
    }

    /** Accessor methods */
    long getNumElem() const {
        return elements.size();
    }

    /**
     * @return An iterator pointing to the first element in the data structure
     */
    vector<DataObject*>::const_iterator beginElements() { return elements.begin();}
    
    /**
     * @return An iterator pointing just past the last element in the data structure
     */
    vector<DataObject*>::const_iterator endElements() { return elements.end();}

    bool isStringReconMethod(){return isStringRecon;};



protected:
    
    /**
     * Encode and transmit synchronization parameters (e.g. synchronization scheme, probability of error ...)
     * to another communicant for the purposes of ensuring that both are using the same scheme.  If the
     * other communicant responds that parameters are the same, returns true.
     * @param commSync The communicant to whom to send the parameters.
     * @param oneWay If set to true, no response is expected from the other communicant (the sync is one-way).
     * @throws SyncFailureException if the parameters don't match between the synchronizing parties.
     */
    virtual void SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay = false);
    
    /**
     * Receive synchronization parameters from another communicant and compare to the current object.
     * Return true iff they are the same (or the other communicant does not care).
     * @param commSync The communicant to whom to send the parameters.
     * @param oneWay If set to true, no response is expected from the other communicant (the sync is one-way).
     * @throws SyncFailureException if the parameters don't match between the synchronizing parties.
     */
    virtual void RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay = false);
    
    SYNC_TYPE SyncID; /** A number that uniquely identifies a given synchronization protocol. */
    
private:
    vector<DataObject *> elements; /** Pointers to the elements stored in the data structure. */

    DataObject* originStr; /** Pointers to the string stored in the data structure. */

    bool isStringRecon = false; /**falg for if it is string reconciliation. */
};


#endif
