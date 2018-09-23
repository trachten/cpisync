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
     * Get a list of edges with vertex start with give string in verStart
     * @param verStart Searching standard for edges start with this string
     * @param changed_shingleSet a set of shingles available at a recursive stage
     * @return a list of edges with vertex and count
     */
    vector<pair<string,int>> getEdges(const string verStart, vector<pair<string,int>> changed_shingleSet);

    /**
     * Recursive function reconstructing string from a shingle set
     * Operation returns multiple strings if Eulerian Cycle exists in the modified De Brujin Graph representation of the shingle set
     * @param changed_shingleSet a set of shingles available at a recursive stage
     * @param curEdge current edge vertex sting
     * @param strColl a vector of strings that is returnable from the shingle set, Returnable object
     * @param str current string
     */
    void shingle2string(vector<pair<string,int>> changed_shingleSet, string curEdge, vector<string>& strCollect, int & str_order, string str="" );

    /**
     * Reconstruct a string from object's shingle set
     * Using backtracking algorithm
     * input order of string to retreive that
     * @param Input int,
     * @return String if input int, int if input string
     */
    pair<string,int> reconstructStringBacktracking(int strOrder=-1);

    /**
     * Subtracts two K_Shingles.
     * -= is destructive and assign
     * @require Shingles have trhe same size k or edge can match
     */
    K_Shingle& operator+=(const K_Shingle& other) const;
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
