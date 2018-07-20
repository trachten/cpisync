/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// IncreCPI.h -- class for sync using interactive CPI algorithm 
#ifndef INCRE_CPI_H
#define INCRE_CPI_H

#include "Auxiliary.h"
#include "CPISync.h"
#include "CPISync_ExistingConnection.h"
/**
 * Implements a data structure for interactively synchronizing sets of
 * data.  The expected amount of communication and computation is linear in the
 * number of differences between the sets, and logarithmic in the overall size of the
 * sets.
 * 
 * This data structure is also designed to allow interruption ... meaning that differences
 * are exchanged as they are encountered, so that an interrupted sync can be continued
 * fairly quickly.  This also means that sets may be updated even if the overall sync fails.
 * 
 * More information is available at:
 * {@link http://ipsit.bu.edu/documents/BUTR2002-01.pdf}
 * Y. Minsky and A. Trachtenberg,
 * "Scalable Set Reconciliation",
 * Allerton, October 2002.
 */

typedef paryTree<CPISync_ExistingConnection> pTree;

class InterCPISync : public SyncMethod {
public:

    /**
     * Specific class constructor.
     * 
     * @param m_bar Maximum number of differences to sync in one round.  Must be &lt;= 2^bits.
     * @param bits  The number of bits used to represent each set element. Must be >= 2.  The data structure
     *    cannot store more than 1^bits elements.
     * @param epsilon An upper bound on the probability of error of the synchronization,
     *    expressed in its negative log.  In other words, the actually probability of error
     *    is upper bounded by 2^-epsilon.  Must be non-negative.
     * 
     *    Internal parameters are tweaked to guarantee this subject to the assumption that there be at least 2 items
     *    in the union of the two sets that are being synchronized.
     * 
     *    @note There may be up to 2^(bit_num) worst case!) constituent synchronizations in one incremental sync,
     *          so the constituent sync error probability enforced is much lower than the overall interactive sync.
     * 
     * @param partition The number of subspaces to partition a subset if synchronization fails.
     *     Must be >= 2 and <2^bits
     */
    InterCPISync(long m_bar, long bits, int epsilon, int partition);

    // destructor
    ~InterCPISync();

    /**
     * Connect as a client to a specific communicant and computes differences between the two (without actually updating them).
     * %R:  Sync_Server must have been called at that communicant.
     * 
     * @param commSync The communicant to whom to connect.
     * @param selfMinusOther A result of reconciliation.  Elements that I have that the other SyncMethod does not.
     * @param otherMinusSelf A result of reconciliation.  Elements that the other SyncMethod has that I do not.
     * @return true iff the connection and subsequent synchronization appear to be successful.
     */
    bool SyncClient(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);

    /**
     * Waits for a client to connect from a specific communicant and computes differences between the two (without actually updating them).
     * 
     * @param commSync The communicant to whom to connect.
     * @param selfMinusOther A result of reconciliation.  Elements that I have that the other SyncMethod does not.
     * @param otherMinusSlef A result of reconciliation.  Elements that the other SyncMethod has that I do not.
     * @return true iff the connection and subsequent synchronization appear to be successful.
     */
    bool SyncServer(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);

    InterCPISync* clone() const;

    /*
     ** update metadata when an element is being added.
     * The maximum number of elements that the data structure can store is 1<<bitNum
     */
    bool addElem(DataObject* newDatum);

    template <typename T>
    bool addElem(T* newDatum);

    // update metadata when an element is being deleted (the element is supplied by index)
    bool delElem(DataObject* newDatum);

    /**
     * Displays some internal information about this object.
     */
    string getName() {
        return string("Interactive CPISync: bitNum=") + toStr(bitNum)
                + ", perr = 2^-" + toStr(probEps) + ", mbar = " + toStr(maxDiff)
                + ", pFactor = " + toStr(pFactor);
               
    }
     int treeRange;
protected:

    pTree *treeNode; /** A tree of CPISync'ed data.  Each tree node is responsible for a specific range of the
                        * space of set data.
                        */
    long bitNum; /** Number of bits used to represent an element of the set that is being synchronized. */
    long maxDiff; /** Maximum number of differences to synchronize (for regular CPIsync) */
    int probEps; /** Negative log of the upper bound on the probability of error for the synchronization. */
    long pFactor; /** The partition factor - into how many partition elements should a node be split. */

    ZZ DATA_MAX; /** Set elements must be within the range 0..data_max-1.  Sample locations are taken between data_max and ZZ_p::modulus() */
    int redundant_k; /** the number of redundant bits needed per CPISync call to get an overall probability of error at most 2^-probEps. */
    ZZ addElemHashID; /** A hash ID shared between the non-recursive and recursive addElem methods.  It is used to place
                       * the new element into the appropriate path of the hash tree. */
    bool useExisting; /** Use Exiting connection for Communication */
    /**
     * Encode and transmit synchronization parameters (e.g. synchronization scheme, probability of error ...)
     * to another communicant for the purposes of ensuring that both are using the same scheme.
     * @param commSync The communicant to whom to send the parameters.
     * @throws SyncFailureException if the parameters don't match between the synchronizing parties.
     */
    void SendSyncParam(Communicant* commSync, bool oneWay = false);

    /**
     * Receive synchronization parameters from another communicant and compare to the current object.
     * Return true iff they are the same.
     * @param commSync The communicant to whom to send the parameters.
     * @throws SyncFailureException if the parameters don't match between the synchronizing parties.
     */
    void RecvSyncParam(Communicant* commSync, bool oneWay = false);
    void createChildren(pTree * treeNode, pTree * tempTree, const ZZ begRange, const ZZ endRange);
private:
    // METHODS

    /**
     * Computes the number of redundant bits needed per CPISync call
     * to get an overall probability of error at most 2^-probEps.
     * @return The number of redundant bits needed.
     */
    int computeRedundant(int epsilon);

    /**
     * Deletes all memory associate with the given tree.
     * @param treeNode A pointer to the root of a tree.
     */
    void deleteTree(pTree *treeNode);

    /* Computes a hash of the given datum of size bit_num, used internally within IntreCPI.
     * @param datum The datum to hash
     * @return A hash of the datum.
     * @note The hash must be between 0 and 2^(bit_num) inclusive.
     * @note Collisions of this hash will render InterCPI less efficient, but should not
     * otherwise cause the synchronization to fail.
     */
    ZZ_p hash(DataObject* datum) const;

    // Recursive versions of public methods
    /**
     * Recursive version of the public method of the same name.  Parameters are the same except those listed.
     * @see Sync_Client(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf)
     * @param treeNode The current node in the tree to synchronize
     * @modifies selfMinusOther - Adds to items discovered to be in my set but not the others'
     * @modifies otherMinusself - Adds to items discovered to be in the others' set but not in mine
     * @return true iff all constituent sync's succeeded
     */
    bool SyncClient(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *&treeNode);

    bool SyncClient(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *treeNode,const ZZ begRange, const ZZ endRange);
    /**
     * Recursive version of the public method of the same name.  Parameters are the same except those listed.
     * @see Sync_Server(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf)
     * @param treeNode The current node in the tree to synchronize
     * @modifies selfMinusOther - Adds to items discovered to be in my set but not the others'
     * @modifies otherMinusself - Adds to items discovered to be in the others' set but not in mine
     *     * @return true iff all constituent sync's succeeded
     */
    bool SyncServer(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *&treeNode);

    bool SyncServer(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *treeNode,const ZZ begRange, const ZZ endRange);

    
    /**
     * Recursive helper function to addElem
     * @param newDatum The datum to add
     * @param treeNode The tree in which to add the element.  The method recursively
     * adds the element to the appropriate children of the node until a node with < m_bar elements is found.
     * @param parent The parent node of this node.
     * @param begRange The beginning item of the current node's range
     * @param endRange The end item of the current node's range
     * @return true iff the addition is successful
     */
    bool addElem(DataObject* newDatum, pTree *&treeNode, pTree *parent, const ZZ &begRange, const ZZ &endRange);

    /**
     * Helper for addElem that creates a new pTree node and populates it with the appropriate elements of
     * the parent in the supplied range.
     */
    bool createTreeNode(pTree * &treeNode, pTree * parent, const ZZ &begRange, const ZZ &endRange);
    void createNewParent(pTree *&parentTree,bool server,Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf);
    bool syncRemainingServer(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *&treeNode);
    bool syncRemainingClient(Communicant* commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, pTree *&treeNode);
    // ... FIELDS
    
    
   
};
#endif
