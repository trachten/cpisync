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
#include "DataObject.h"
#include "ProcessData.h"

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;

typedef unsigned long idx_t;

class K_Shingle {
public:
    // Communicant needs to access the internal representation of an kshingle to send it and receive it
    friend class Communicant;

    /**
     * Construct a K_Shingle set object with k as each shingle size
     * @param k fixing shingle size to be k
     */
    K_Shingle(const size_t shingle_size, const char stop_word = '$');

    // Default deconstructor
    ~K_Shingle();

    bool inject(const string str) {
        clear_shingleSet();
        orig_string = stopword + str + stopword;
        return create(str);
    };

    /**
     * Increment the edge count of the ShingleSet, Creates no copy
     * Works on object
     * @param ver shingle
     */
    void incrementEdgeCount(const string ver, map<string, idx_t> &shingle_map);


    /**
     * 0 Based order
     * Reconstruct a string from object's shingle set
     * Using backtracking algorithm
     * input order of string to retreive that
     * @param Input int,
     * @return String if input int, int if input string
     * Fail to return a string, second would be 0
     */
    pair<string, idx_t> reconstructStringBacktracking(idx_t str_coll_tar = 0);

    // get methods
    /**
     * @return The number of element in she shingle set
     */
    size_t getSetSize() const {
        return shingleSet.size();
    }

    /**
     * @return The bit size of a shingle set
     */
    size_t getElemSize() const {
        return sizeof(DataObject*);
        //return sizeof(pair<string, idx_t>);
    }

    char getStopWord() const {
        return stopword;
    }

    string getOriginString() {
        if (orig_string.empty() || orig_string == "$$") return "";
        return orig_string.substr(1, orig_string.size() - 2);
    }

    vector<pair<string, idx_t>> getShingleSet() {
        return shingleSet;
    }

    /**
     * Interact with sending set elements as string
     * @return
     */
    vector<string> getShingleSet_str() { return shingleSet_str; };

    size_t getshinglelen_str() {return shingleSet_str.back().size();};

    /**
     * Interact with reciving set element update as string
     */
    void updateShingleSet_str(string shingle);

    // Delete and reinsert
    void clear_shingleSet() {
        shingleSet.clear();
        shingleSet_str.clear();
    };

private:
    // local data

    //default constructor
    K_Shingle();


    // k and stopword better be the same between two hosts, or should be transferred.
    size_t k;  //shingle size

    const char stopword;  // default stop word is "$"

    // resetable parameters
    vector<pair<string, idx_t>> shingleSet; // transfer shingleSet to other host
    vector<string> shingleSet_str; // mirror variable, used to sedn to another hose in string format

    string orig_string;  // original string with stopwords on both ends

    /**
     * GET THE NEXT POSSIBLE EDGES 
     * @param changed_shingleSet pair<edge idx,edge occurrence>
     * @return index of next edges to look at
     */
    vector<idx_t> getEdgeIdx(const string verStart, vector<idx_t> changed_shingleOccur);

    /**
     * Iterative function reconstructing string from a shingle set
     * Operation returns multiple strings if Eulerian Cycle exists in the modified De Brujin Graph representation of the shingle set
     * @param changed_shingleSet a set of shingles available at a recursive stage
     * @param curEdge current edge vertex sting
     * @param strColl a vector of strings that is returnable from the shingle set, Returnable object
     * @param str current string
     * @return whether this process is successfully.
     */
    bool shingle2string(vector<pair<string, idx_t>> changed_shingleOccur, string curEdge, idx_t &strCollect_ind,
                        idx_t &str_order, string &final_str, string str = "");

    /**
     * create a set of k-shingles from String str
     * This operation always succeed
     * @param str Original string
     */
    bool create(const string str);

    inline bool emptyState(vector<idx_t> state) {
        for (idx_t i = 0; i < state.size(); ++i) {
            if (state[i] > 0) return false;
        }
        return true;
    }

    void insert(pair<string, idx_t> Elem) {
        shingleSet.push_back(Elem);
    };
};

#endif //CPISYNCLIB_KSHINGLING_H
