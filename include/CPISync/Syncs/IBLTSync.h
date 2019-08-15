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

#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/IBLT.h>

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
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool addElem(shared_ptr<DataObject> datum) override;
    bool delElem(shared_ptr<DataObject> datum) override;
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
