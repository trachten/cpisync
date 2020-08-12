//
// Created by Shubham Arora on 7/20/20.
//

#ifndef CPISYNC_IBLTMULTISET_H
#define CPISYNC_IBLTMULTISET_H

#include "IBLT.h"

// this prime is used for modulus operations
const long int LARGE_PRIME = 982451653;

class IBLTMultiset: public IBLT {
public:

    // Communicant needs to access the internal representation of an IBLT to send and receive it
    friend class Communicant;

    /**
     * Constructs an IBLT object with size relative to expectedNumEntries.
     * @param expectedNumEntries The expected amount of entries to be placed into the IBLT
     * @param _valueSize The size of the values being added, in bits
     * @param isMultiset Is the IBLT going to store multiset values, default is false
     */
    IBLTMultiset(size_t expectedNumEntries, size_t _valueSize);

    IBLTMultiset();

    /**
     * Inserts a key-value pair to the IBLT.
     * This operation always succeeds.
     * @param key The key to be added
     * @param value The value to be added
     * @require The key must be distinct in the IBLT
     */
    void insert(ZZ key, ZZ value) override;

    /**
     * Erases a key-value pair from the IBLT.
     * This operation always succeeds.
     * @param key The key to be removed
     * @param value The value to be removed
     */
    void erase(ZZ key, ZZ value) override;

    /**
     * Produces the value s.t. (key, value) is in the IBLT.
     * This operation doesn't always succeed.
     * This operation is destructive, as entries must be "peeled" away in order to find an element, i.e.
     * entries with only one key-value pair are subtracted from the IBLT until (key, value) is found.
     * @param key The key corresponding to the value returned by this function
     * @param result The resulting value corresponding with the key, if found.
     * If not found, result will be set to 0. result is unchanged iff the operation returns false.
     * @return true iff the presence of the key could be determined
     */
    bool get(ZZ key, ZZ& result);

    /**
     * Produces a list of all the key-value pairs in the IBLT.
     * With a low, constant probability, only partial lists will be produced
     * Listing is destructive, as the same peeling technique used in the get method is used.
     * Will remove all key-value pairs from the IBLT that are listed.
     * @param positive All the elements that could be inserted.
     * @param negative All the elements that were removed without being inserted first.
     * @return true iff the operation has successfully recovered the entire list
     */
    bool listEntries(vector<pair<ZZ, ZZ>>& positive, vector<pair<ZZ, ZZ>>& negative);

//    /**
//     * Convert IBLT to a readable string
//     * @return string
//    */
//    string toString() const;

    /**
     * Subtracts two IBLTs.
     * -= is destructive and assigns the resulting iblt to the lvalue, whereas - isn't. -= is more efficient than -
     * @param other The IBLT that will be subtracted from this IBLT
     * @require IBLT must have the same number of entries and the values must be of the same size
     */
    IBLTMultiset operator-(const IBLTMultiset& other) const;
    IBLTMultiset& operator-=(const IBLTMultiset& other);

    /**
     * @return the number of cells in the IBLT. Not necessarily equal to the expected number of entries
     */
    size_t size() const;

    /**
     * @return the size of a value stored in the IBLT.
     */
    size_t eltSize() const;

    vector<hash_t> hashes; /* vector for all hashes of sets */

    string toString() const override;

    void reBuild(string &inStr) override;

private:
    /**
     * Performs the actual insertion or deletion
     * @param plusOrMinus The indicator of whether it is insert (1) or delete (-1)
     * @param key The key to insert or delete
     * @param value The value to insert or delete
     */
    void _insertModular(long plusOrMinus, const ZZ &key, const ZZ &value);

//    class HashTableEntry {
//    public:
//        // Net insertions and deletions that mapped to this cell
//        long count;
//
//        // The bitwise xor-sum of all keys mapped to this cell
//        ZZ keySum;
//
//        // The bitwise xor-sum of all keySum checksums at each allocation
//        hash_t keyCheck;
//
//        // The bitwise xor-sum of all values mapped to this cell
//        ZZ valueSum;
//
//////         Returns whether the entry contains just one insertion or deletion
////        bool isPure() const;
//
//        // Returns whether the entry contains just insertions or deletions of only one key-value pair
//        bool isPure() const;
//
//        // Returns whether the entry is empty
//        bool empty() const;
//    };

    class HashTableEntry : public IBLT::HashTableEntry {
    public:
        bool isPure() const override;
    };


    // vector of all entries
    vector<HashTableEntry> hashTable;

    // the value size, in bits
    size_t valueSize;

};

#endif //CPISYNC_IBLTMULTISET_H
