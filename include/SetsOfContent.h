//
// Created by Bowen Song on 12/9/18.
//

#ifndef CPISYNCLIB_SETSOFCONTENT_H
#define CPISYNCLIB_SETSOFCONTENT_H


#include <vector>
#include <string>
#include <NTL/ZZ.h>
#include "kshingling.h"
#include "StrataEst.h"
#include "CPI.h"
#include "SyncMethod.h"
#include "DataObject.h"
#include <algorithm>
#include <NTL/ZZ_p.h>
#include "Exceptions.h"
#include "InterCPISync.h"
#include "IBLTSync_SetDiff.h"
#include "ProbCPISync.h"
#include "FullSync.h"

#include "ForkHandle.h" // tobe removed

using std::vector;
using std::hash;
using std::string;
using namespace NTL;

/**
 * first is the firstr part of a shingle
 * second is the second part of the origion
 * occurrence, how manny time this occrred
 * origin is the last origining substring
 */
struct shingle_hash{ // TODO: change all size_t to unsigned int to cut doen element size by half. see if it limites the hashes
    size_t first, second, occurr, groupID, cycleVal, lvl;
};
//Compare and help order struct shingle_hash from a vector
static bool operator<(const shingle_hash& a, const shingle_hash& b) { return a.first < b.first; };
//Compare and help differetiate struct shingle_hash
static bool operator==(const shingle_hash& a, const shingle_hash& b) {
    return a.first == b.first and a.second == b.second and a.groupID == b.groupID and a.occurr == b.occurr and
           a.cycleVal == b.cycleVal and a.lvl == b.lvl;
};

static shingle_hash ZZtoShingleHash(const ZZ& zz){
    shingle_hash shingle;
    BytesFromZZ((uint8_t *) &shingle,zz, sizeof(shingle_hash));
    return shingle;
}

static vector<shingle_hash> ZZtoShingleHash_vec(const vector<ZZ>& zz_vec){
    vector<shingle_hash> ZZ_VEC;
    for(auto zz : zz_vec) {
        shingle_hash shingle;
        BytesFromZZ((uint8_t *) &shingle, zz, sizeof(shingle_hash));
        ZZ_VEC.push_back(shingle);
    }
    return ZZ_VEC;
}

static ZZ ShingleHashtoZZ(shingle_hash shingle) {
    char* my_s_bytes = reinterpret_cast<char*>(&shingle);
    return ZZFromBytes((const uint8_t *) my_s_bytes, sizeof(shingle_hash));
}

static vector<ZZ> ShingleHashtoZZ_vec(vector<shingle_hash> shingle_vec) {
    vector<ZZ> SHINGLE_VEC;
    for(auto shingle : shingle_vec) {
        char *my_s_bytes = reinterpret_cast<char *>(&shingle);
        SHINGLE_VEC.push_back(ZZFromBytes((const uint8_t *) my_s_bytes, sizeof(shingle_hash)));
    }
    return SHINGLE_VEC;
}



class SetsOfContent : public SyncMethod {
public:
    SetsOfContent(size_t terminal_str_size = 50, size_t levels = NOT_SET);

    ~SetsOfContent();

    string retriveString();

// functions for SyncMethods
    bool addStr(DataObject* str, vector<DataObject*> &datum,  bool sync) override;

    bool SyncClient(const shared_ptr<Communicant> &commSync, shared_ptr<SyncMethod> &setHost) override;

    bool SyncServer(const shared_ptr<Communicant> &commSync, shared_ptr<SyncMethod> &setHost) override;

    string getName() override {return "Sets of Content";}

    /**
     * Get the terminal strings to reconcile with another set
     * @return a vector of terminla string
     */
    vector<string> getTerminalDiffStr(vector<shingle_hash> diff_shingle);

    //getShinglesAt
    vector<ZZ> getShingles_ZZ() {
        vector<ZZ> res;
        for(auto treelvl : myTree) {
            for (auto item:treelvl) {
                res.push_back(ShingleHashtoZZ(item));
            }
        }
        return res;
    };


protected:
    bool useExisting; /** Use Exiting connection for Communication */

private:

    string myString; // original input string
    size_t TermStrSize, Levels, Partition;

    vector<DataObject*> setPointers;

    // each level: store string and their conter-part on the other side
    vector<map<size_t, size_t>> conformingPair;

    vector<vector<shingle_hash>> myTree, theirTree; // the hash shingle tree

    map<size_t, string> dictionary; // TODO: transfer into index of the string to save auxilary space

    //requests
    vector<size_t> Req_Dict, Req_Group, Rev_Dict, send_Cyc, get_Cyc;

    map<size_t,bool> Rev_Group;

    vector<string> send_Dict,get_Dict;

    size_t str_to_hash(string str) {
        return std::hash<std::string>{}(str);
    };

    /**
     * Create content dependent partitions based on the input string
     * Update Dictionary
     * @param str origin string to be partitioned
     * @param win_size partition size is 2*win_size
     * @param space smaller-more partitions
     * @param shingle_size inter-relation of the string
     * @return vector of substring hashes in origin string order
     */
    vector<size_t> create_HashSet(string str,size_t win_size, size_t space=NOT_SET, size_t shingle_size=NOT_SET);

    vector<size_t> local_mins(vector<size_t> hash_val, size_t win_size);

    /**
     * Insert string into dictionary
     * @param str a substring
     * @return hash of the string
     * @throw if there is duplicates, suggest using new/multiple hashfunctions
     */
    size_t add_to_dictionary(string str);

    inline size_t min_between(const vector<size_t> &nums,size_t from, size_t to){
        if (nums.empty()) throw invalid_argument("min function does not take empty vector");
        size_t min = nums[0];
        for (size_t i = from; i <= to; ++i) {
            if (nums[i]<min) min = nums[i];
        }
        return min;
    }

    // extract the unique substring hashes from the shingle_hash vector
    vector<size_t> unique_substr_hash(vector<shingle_hash> hash_set){
        set<size_t> tmp;
        for(shingle_hash item : hash_set){
            tmp.insert(item.first);
            tmp.insert(item.second);
        }
        vector<size_t> tmp_vec(tmp.begin(),tmp.end());
        return tmp_vec;
    }

    void update_tree(vector<size_t> hash_vector, size_t level, bool isComputeCyc);

    size_t get_group_signature(vector<size_t> strordered_hashset);

    // functions for backtracking
    /**
     * peice shingle_hashes back to a vector of hashes in the string order
     * @param shingle_set
     * @param str_order
     * @param final_str a hash train in string order
     */
    bool shingle2hash_train(vector<shingle_hash> shingle_set,size_t &str_order, vector<size_t> &final_str);

    vector<size_t> get_nxt_edge_idx(size_t current_edge, vector<shingle_hash>changed_shingleOccur);

    bool empty_state(vector<shingle_hash> state);
    /**
     * compute cycle number based on ordered substring_hash and shingle_hashes
     * Set the cycle num at the head of the shingle_hash struct and outputs the number
     * @param shingle_set shingle_hases in lexicographic order
     * @param strordered_substr_hash hash in string order
     * @return cycle number
     */
    size_t set_cyc_val(vector<shingle_hash>& shingle_set, const vector<size_t> strordered_substr_hash);

    /**
     * sub fucntion for "get_all_strs_from" extracting string from a group of shigle_hashes
     * @param shingle_set shinge_hashes from a group
     * @return the string of that group
     */
    string get_str_from(vector<shingle_hash> shingle_set);

    /**
     * get the full shigle_hash set from a level and spratet them by groups to feed into "get_str_from"
     * @param level_shingle_set shingle_hashes from a level
     * @return reconstruct string from the ground lvl
     */
    vector<string> get_all_strs_from(vector<shingle_hash> level_shingle_set);

    /**
     *
     * @param shingle_hash_theirs
     * @param shingle_hash_mine
     * @param groupIDs
     * @return hashes of unknown
     */
    vector<size_t> single_out(vector<shingle_hash> shingle_hash_theirs, vector<shingle_hash> shingle_hash_mine, vector<size_t>& groupIDs);


    void redo_tree_with_cyc();

    vector<size_t> get_string_cycles();

        // functions for Sync Methods

    void SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay = false) override;

    void RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay = false) override;

    void configure(shared_ptr<SyncMethod>& setHost, GenSync::SyncProtocol SyncProtocol = GenSync::SyncProtocol::IBLTSyncSetDiff);

    bool reconstructString(DataObject* & recovered_string, const list<DataObject *> & theirsMinusMine, const list<DataObject *> & mineMinusTheirs)override;
};
#endif //CPISYNCLIB_SETSOFCONTENT_H
