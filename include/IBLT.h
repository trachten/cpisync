//
// Created by eliezer pearl on 7/9/18.
// Heavily based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research.
//

#ifndef CPISYNCLIB_IBLT_H
#define CPISYNCLIB_IBLT_H

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include "Auxiliary.h"
#include "MurmurHash3.h"

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;

const long N_HASH = 4;
const long N_HASHCHECK = 11;

typedef unsigned long int hashVal;

/*
 * IBLT (Invertible Bloom Lookup Table) is a data-structure designed to add
 * probabilistic invertibility to the standard Bloom Filter data-structure.
 *
 * A complete description of the IBLT data-structure can be found in: 
 * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables." 
 * arXiv preprint arXiv:1101.2245 (2011).
 */

class IBLT {
public:
    // default constructor
    IBLT(unsigned long expectedNumEntries);
    
    // default destructor
    ~IBLT();
    
    /**
     * Inserts a key-value pair to the IBLT.
     * This operation always succeeds.
     * @param key The key to be added
     * @param value The value to be added
     * @require The key must be distinct in the IBLT
     */
    void insert(ZZ key, ZZ value);
    
    /**
     * Erases a key-value pair from the IBLT.
     * This operation always succeeds.
     * @param key The key to be removed
     * @param value The value to be removed
     */
    void erase(ZZ key, ZZ value);
    
    /**
     * Produces the value s.t. (key, value) is in the IBLT.
     * This operation doesn't always succeed.
     * This operation is destructive.
     * @param key The key corresponding to the value returned by this function
     * @param result The resulting value corresponding with the key, if found.
     * If not found, result will be set to 0. result is unchanged iff the operation returns false.
     * @return true iff the presence of the key could be determined
     */
    bool get(ZZ key, ZZ& result);
    
    /**
     * Produces a list of all the key-value pairs in the IBLT.
     * With a low, constant probability, only partial lists will be produced
     * Listing is destructive. Will remove all key-value pairs from the IBLT that are listed.
     * @param positive All the elements that could be inserted.
     * @param negative All the elements that were removed without being inserted first.
     * @return true iff the operation has successfully recovered the entire list
     */
    bool listEntries(vector<pair<ZZ, ZZ>>& positive, vector<pair<ZZ, ZZ>>& negative);

    // @require iblts same size and params
    // -= more efficient
    /**
     * Subtracts two IBLTs.
     * -= is destructive, whereas - isn't. -= is more efficient than -
     */
    IBLT operator-(const IBLT& other) const;
    IBLT& operator-=(const IBLT& other);

    // for debugging purposes only.
    std::string DumpTable() const;
private:
    // local data

    // Helper function for insert and erase
    void _insert(long plusOrMinus, ZZ key, ZZ value);

    // Returns a pair of the first and second unique hashes of zz.
    //static pair<hashVal, hashVal> initialHash(ZZ zz);

    // Returns the kk-th unique hash of the zz that produced initial.
    //static hashVal hashK(const pair<hashVal, hashVal>& initial, long kk);
    static hashVal hashK(const ZZ& item, long kk);
    static hashVal _hash(hashVal initial, long kk);
    // Represents each entry in the iblt
    class HashTableEntry
    {
    public:
        // Net insertions and deletions that mapped to this cell
        long count;

        // The bitwise xor-sum of all keys mapped to this cell
        ZZ keySum;

        // The bitwise xor-sum of all keySum checksums at each allocation
        hashVal keyCheck;

        // The bitwise xor-sum of all values mapped to this cell
        ZZ valueSum;

        // Returns whether the entry contains just one insertion or deletion
        bool isPure() const;
        //bool isPure(const pair<hashVal, hashVal>& initial) const;

        // Returns whether the entry is empty
        bool empty() const;
    };

    // vector of all entries
    vector<HashTableEntry> hashTable;
};

#endif //CPISYNCLIB_IBLT_H
