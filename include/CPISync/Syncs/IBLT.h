/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/9/18.
// Based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research.
//

#ifndef CPISYNCLIB_IBLT_H
#define CPISYNCLIB_IBLT_H

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include <CPISync/Aux/Auxiliary.h>

using std::hash;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;
using namespace NTL;

// The number of hashes used per insert
const long N_HASH = 4;

// The number hash used to create the hash-check for each entry
const long N_HASHCHECK = 11;

// Shorthand for the hash type
typedef unsigned long int hash_t;

/*
 * IBLT (Invertible Bloom Lookup Table) is a data-structure designed to add
 * probabilistic invertibility to the standard Bloom Filter data-structure.
 *
 * A complete description of the IBLT data-structure can be found in: 
 * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables." 
 * arXiv preprint arXiv:1101.2245 (2011).
 */
class IBLT
{
public:
  // Communicant needs to access the internal representation of an IBLT to send and receive it
  friend class Communicant;

  /**
     * Constructs an IBLT object with size relative to expectedNumEntries.
     * @param expectedNumEntries The expected amount of entries to be placed into the IBLT
     * @param _valueSize The size of the values being added, in bits
     */
  IBLT(size_t expectedNumEntries, size_t _valueSize);

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
     * This operation is destructive, as entries must be "peeled" away in order to find an element, i.e.
     * entries with only one key-value pair are subtracted from the IBLT until (key, value) is found.
     * @param key The key corresponding to the value returned by this function
     * @param result The resulting value corresponding with the key, if found.
     * If not found, result will be set to 0. result is unchanged iff the operation returns false.
     * @return true iff the presence of the key could be determined
     */

  void insertIBLT(IBLT &chldIBLT, hash_t &chldHash);
  void eraseIBLT(IBLT &chldIBLT, hash_t &chldHash);

  bool get(ZZ key, ZZ &result);

  /**
     * Produces a list of all the key-value pairs in the IBLT.
     * With a low, constant probability, only partial lists will be produced
     * Listing is destructive, as the same peeling technique used in the get method is used.
     * Will remove all key-value pairs from the IBLT that are listed.
     * @param positive All the elements that could be inserted.
     * @param negative All the elements that were removed without being inserted first.
     * @return true iff the operation has successfully recovered the entire list
     */
  bool listEntries(vector<pair<ZZ, ZZ>> &positive, vector<pair<ZZ, ZZ>> &negative);

  /**
   * Convert IBLT to string
   * @return string
   */
  string toString() const;

  /**
   * Rebuild IBLT from string infos
   * @param inStr IBLT hashtableentries in string type, converted by toString function above
   */
  void reBuild(string &inStr);

  /**
   * Insert a set into chldIBLT and put chldIBLT into current IBLT
   * @param tarSet target set to be added to IBLT
   */
  void insertIBLT(multiset<DataObject *> tarSet, size_t elemSize);

  /**
   * Delete an IBLT by a given set
   * @param tarSet the target set to be deleted
   * @param elemSize size of element in the chld set
   */
  void eraseIBLT(multiset<DataObject *> tarSet, size_t elemSize);

  static ZZ getHash(multiset<DataObject *> tarSet);
  /**
     * Subtracts two IBLTs.
     * -= is destructive and assigns the resulting iblt to the lvalue, whereas - isn't. -= is more efficient than -
     * @param other The IBLT that will be subtracted from this IBLT
     * @require IBLT must have the same number of entries and the values must be of the same size
     */
  IBLT operator-(const IBLT &other) const;
  IBLT &operator-=(const IBLT &other);

  /**
     * @return the number of cells in the IBLT. Not necessarily equal to the expected number of entries
     */
  size_t size() const;

  /**
     * @return the size of a value stored in the IBLT.
     */
  size_t eltSize() const;

  vector<hash_t> hashes; /* vector for all hashes of sets */

private:
  // local data

  // default constructor - no internal parameters are initialized
  IBLT();

  // Helper function for insert and erase
  void _insert(long plusOrMinus, ZZ key, ZZ value);

  // Returns the kk-th unique hash of the zz that produced initial.
  static hash_t _hashK(const ZZ &item, long kk);
  static hash_t _hash(const hash_t &initial, long kk);
  static hash_t _setHash(multiset<DataObject *> &tarSet);

  // Represents each entry in the iblt
  class HashTableEntry
  {
  public:
    // Net insertions and deletions that mapped to this cell
    long count;

    // The bitwise xor-sum of all keys mapped to this cell
    ZZ keySum;

    // The bitwise xor-sum of all keySum checksums at each allocation
    hash_t keyCheck;

    // The bitwise xor-sum of all values mapped to this cell
    ZZ valueSum;

    // Returns whether the entry contains just one insertion or deletion
    bool isPure() const;

    // Returns whether the entry is empty
    bool empty() const;
  };

  // vector of all entries
  vector<HashTableEntry> hashTable;

  // the value size, in bits
  size_t valueSize;
};

#endif //CPISYNCLIB_IBLT_H
