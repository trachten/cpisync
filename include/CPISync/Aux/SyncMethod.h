/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#ifndef SYNC_METHODS_H
#define SYNC_METHODS_H

#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>
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
     * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference). When this is called
     * the internal stats about the previous sync are reset
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
     * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference). When this is called the internal stats
     * about the previous sync are reset
     *
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

        SyncStats(){
        	for(int ii = 0; ii < 5; ii++)
        		dataArray[ii] = 0;
        }

        ~SyncStats() = default;

        /**
         * Resets specified counter to 0
         */
        inline void reset(StatID statID){
        	int myIndex = enumToIndex(statID);

        	//Valid index that isn't all
        	if(myIndex < 5)
        		dataArray[myIndex] = 0;

        	//All
        	else if(myIndex == 5)
				for(int ii = 0; ii < 5; ii++)
					dataArray[ii] = 0;

        }

        /**
         * @param statID The stat you would like to return
         * @return The stat specified by statID
         * Does not support ALL
         */
        inline double getStat(StatID statID){
			return dataArray[enumToIndex(statID)];
        }

        /**
         * Increment the specified stat by the specified amount
         * @param statID The stat to increment
         * @param incr how much to increment the given stat by
         */
        inline void increment(StatID statID, double incr){
			int myIndex = enumToIndex(statID);

			//Byte xmit and recv
			if(myIndex < 3 && myIndex >= 0)
				dataArray[myIndex] += floor(incr);

			//Timers
			else if(myIndex < 5)
				dataArray[myIndex] += incr;

			//All
			else if(myIndex == 5) {
				dataArray[0] += floor(incr);
				dataArray[1] += floor(incr);
				dataArray[2] += incr;
				dataArray[3] += incr;
				dataArray[4] += incr;
			}

        }

        /**
         * starts a timer for the specified StatID. Only works for time type StatIDs
         * @param timerID The id of the timer you would like to start
         */
        inline void timerStart(StatID timerID){
			int myIndex = enumToIndex(timerID);

			if(myIndex < 5)
				startTimeArray[myIndex - 2] = std::chrono::high_resolution_clock::now();

			else if (myIndex == 5)
				for(int ii = 2; ii < 5; ii++)
					startTimeArray[myIndex] = std::chrono::high_resolution_clock::now();
        }

        /**
         * End the timer for the specified "time" type stat adding the time since the start to the relevant time stat
         * @param timerID The id of the timer you would like to end
         * @requires timerStart must have been called for the same StatID before use
         */
        inline void timerEnd(StatID timerID){
			int myIndex = enumToIndex(timerID);

			//Comm, idle or comp time
			if(myIndex < 5)
				dataArray[myIndex] += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - startTimeArray[myIndex-2]).count() * 1e-6);

			//Update all the time stats
			else
				for(int ii = 2; ii < 5;ii++)
					dataArray[ii] += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - startTimeArray[ii-2]).count() * 1e-6);
        }

        /**
         * converts an enum to its index in the dataArray
         * @param myEnum the enum to convert
         * @return the index in the array
         */
        inline int enumToIndex(StatID myEnum){
			switch(myEnum) {
				case SyncMethod::SyncStats::XMIT:
					return 0;
				case SyncMethod::SyncStats::RECV:
					return 1;
				case SyncMethod::SyncStats::COMM_TIME:
					return 2;
				case SyncMethod::SyncStats::IDLE_TIME:
					return 3;
				case SyncMethod::SyncStats::COMP_TIME:
					return 4;
				case SyncMethod::SyncStats::ALL:
					return 5;
				default:
					Logger::error("Invalid enum type specified");
			}
        }

        /**
         * @return The total time required for this sync to complete
         */
        double totalTime(){ return dataArray[2] + dataArray[3] + dataArray[4]; };

    private:

    	/**
         * 0 = xmitBytes; The total amount of bytes that this sync has transmitted
         * 1 = recvBytes; The total amount of bytes that this sync has received
         * 2 = commTime; The total amount of time that this time has spent sending and receiving through sockets
         * 3 = idleTime; The total time spent waiting for your peer (listening, waiting for computation etc)
         * 4 = compTime; The total amount time taken to finish the computation for this sync
         */
        double dataArray[5];

        /**
         * An array of the start times for each timer
         * 0 = commStart
         * 1 = idleStart
         * 2 = compStart
         */
		std::chrono::high_resolution_clock::time_point startTimeArray[3];
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
