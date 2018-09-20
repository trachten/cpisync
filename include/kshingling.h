//
// Created by Bowen Song on 9/15/18.
// Based on
//

#ifndef CPISYNCLIB_KSHINGLING_H
#define CPISYNCLIB_KSHINGLING_H

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include "Auxiliary.h"

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;

const string stopword = "$";

class K_Shingle {
public:
    // Communicant needs to access the internal representation of an kshingle to send it and receive it
    friend class Communicant;

    /**
     * Construct a K_Shingle set object with k as each shingle size
     * @param k fixing shingle size to be k
     */
    K_Shingle(const string str, const size_t k);

    // Default deconstructor
    ~K_Shingle();

    /**
     * create a set of k-shingles from String str
     * This operation always succeed
     * @param str Original string
     */
    void _create();

    /**
     * Produces the value s.t. (key, value) is in the shingle set.
     * This operation cannot always find a match.
     * @param ver The ver corresponding to the value returned by this function
     * @param edge The resulting value corresponding with the key, if found.
     * If not found, result will be set to 0. result is unchanged iff the operation returns false.
     * @return An edge of pair<vertex,occurrance> if vertex == ver, ZZ& edge is the result
     */
    bool get(const string ver, pair<string,int>& edge);

    /**
     * Increment the edge count of the ShingleSet, Creates no copy
     * Works on object
     * @param ver shingle
     */
    void incrementEdgeCount(const string ver);

    /**
     * Reconstruct a string from a shingle set
     * Operation fails if Eulerian Cycle exists in the modified De Brujin Graph representation of the shingle set
     * @return True iff a string is uniquely decoded from shingle set
     */
    bool reconstructString();

    /**
     * Subtracts two K_Shingles.
     * -= is destructive and assign
     * @require Shingles have trhe same size k or edge can match
     */
    K_Shingle operator+=(const K_Shingle& other) const;
    K_Shingle& operator-=(const K_Shingle& other);

    /**
     * @return The number of element in she shingle set
     */
    size_t set_size() const;

    /**
     * @return The bit size of shingle set
     */
    size_t set_bit_size() const;

private:
    // local data

    //default constructor
    K_Shingle();

    int k;  //shingle size

    vector<pair<string,int>> shingleSet;  // shingle set

    string orig_string;  // original string

    const string stopword = "$";  // default stop word is "$"




};

#endif //CPISYNCLIB_KSHINGLING_H
