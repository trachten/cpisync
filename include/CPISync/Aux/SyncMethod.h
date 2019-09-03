/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#ifndef SYNC_METHODS_H
#define SYNC_METHODS_H

#include <algorithm>
#include <vector>
#include <memory>
#include <CPISync/Communicants/Communicant.h>

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
    virtual bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) {
        mySyncStats.reset(SyncStats::ALL);
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
    virtual bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) {
        mySyncStats.reset(SyncStats::ALL);
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
    virtual bool addElem(shared_ptr<DataObject> datum) { elements.push_back(datum); return true; };

    /**
     * Delete an element from the data structure that will be performing the synchronization.
     * @param datum The element to delete.
     * @return true iff the removal was successful
     */
    virtual bool delElem(shared_ptr<DataObject> datum) { 
        long int before = elements.size();
        elements.erase(std::remove(elements.begin(), elements.end(), datum), elements.end());
        return before > elements.size(); // true iff there were more elements before removal than after
    };

    // INFORMATIONAL
    /**
     * @return A human-readable name for the synchronization method.
     */
    virtual string getName() = 0;

    /** Accessor methods */
    long getNumElem() const {
        return elements.size();
    }
        /**
     * Deal with elements in OtherMinusSelf after finishing a specific sync function.
     * Works only when data type for elements is SET
     * @param *add function pointer to the addElem function in GenSync class
     * @param *del function pointer to the delElem function in GenSync class
     * @param otherMinusSelf list of dataObjects, received from every specific sync function
     * @param myData list of dataObjects, containing all elems saved in the data structure
     **/
    template <class T>
    static void postProcessing_SET(list<shared_ptr<DataObject>> otherMinusSelf, list<shared_ptr<DataObject>> myData, void (T::*add)(shared_ptr<DataObject>), bool (T::*del)(shared_ptr<DataObject>), T *pGenSync)
    {
        for (auto elem : otherMinusSelf)
        {
            (pGenSync->*add)(elem);
        }
    }


    /**
     * @return An iterator pointing to the first element in the data structure
     */
    vector<shared_ptr<DataObject>>::const_iterator beginElements() { return elements.begin();}
    
    /**
     * @return An iterator pointing just past the last element in the data structure
     */
    vector<shared_ptr<DataObject>>::const_iterator endElements() { return elements.end();}



    /**
     * A class containing statistics about a sync and methods for modifying these stats
     */
    class SyncStats{
    public:

        enum StatID { XMIT, RECV, COMM_TIME, IDLE_TIME, COMP_TIME, ALL }; //Stat specifiers

        SyncStats();
        ~SyncStats() = default;

        /**
         * Resets specified counter to 0
         */
        void reset(StatID statID);

        /**
         * @param statID The stat you would like to return
         * @return The stat specified by statID
         * Does not support ALL
         */
        double getStat(StatID statID);

        /**
         * Increment the specified stat by the specified amount
         * @param statID The stat to increment
         * @param incr how much to increment the given stat by
         */
        void increment(StatID statID, double incr);

        /**
         * starts a timer for the specified StatID. Only works for time type StatIDs
         * @param timerID The id of the timer you would like to start
         */
        void timerStart(StatID timerID);

        /**
         * End the timer for the specified "time" type stat adding the time since the start to the relevant time stat
         * @param timerID The id of the timer you would like to end
         * @requires timerStart must have been called for the same StatID before use
         */
        void timerEnd(StatID timerID);

        /**
         * @return The total time required for this sync to complete
         */
        double totalTime(){ return commTime + idleTime + compTime; };

    private:
        long xmitBytes; /** The total amount of bytes that this sync has transmitted */
        long recvBytes; /** The total amount of bytes that this sync has received*/

        std::chrono::high_resolution_clock::time_point commStart;
        double commTime; /** The total amount of time that this time has spent sending and receiving through sockets */

        std::chrono::high_resolution_clock::time_point idleStart;
        double idleTime; /** The total time spent waiting for your peer (listening, waiting for computation etc) */

        std::chrono::high_resolution_clock::time_point compStart;
        double compTime; /** The total amount time taken to finish the computation for this sync */
    };

    /**
     * An object that collects stats about a sync
     */
    SyncStats mySyncStats;

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
    vector<shared_ptr<DataObject>> elements; /** Pointers to the elements stored in the data structure. */
};


#endif
