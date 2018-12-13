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
struct shingle_hash{
    size_t first, second, occurr, groupID, cycleVal, lvl;
};
//Compare and help order struct shingle_hash from a vector
static bool operator<(const shingle_hash& a, const shingle_hash& b) { return a.first < b.first; };
//Compare and help differetiate struct shingle_hash
//static bool operator==(const shingle_hash& a, const shingle_hash& b){
//    return a.first == b.first and a.second == b.second and a.origin == b.origin and a.occurr == b.occurr;
//};

class SetsOfContent : public SyncMethod {
public:
    SetsOfContent(size_t terminal_str_size = 50, size_t levels = NOT_SET);

    ~SetsOfContent() = default;

    void injectString(string str);

    string retriveString();

// functions for  SyncMethods
    bool addStr(DataObject* str, vector<DataObject*> &datum,  bool sync) override;

    string getName() override {return "Sets of Content";}

    /**
     * Get the terminal strings to reconcile with another set
     * @return a vector of terminla string
     */
    multiset<string> getTerminalStr();

    //getShinglesAt
    multiset<size_t> getShinglesSum() {
        multiset<size_t> res;
        for(auto treelvl : tree) {
            for (auto item:treelvl) {
                res.insert(item.first * 2 + item.occurr * 3 + item.second * 4 + item.groupID * 5 + item.cycleVal*6+item.lvl*7);
            }
        }
        return res;
    };
private:

    string myString; // original input string
    size_t TermStrSize, Levels, Partition;

    // each level: store string and their conter-part on the other side
    vector<map<size_t, size_t>> conformingPair;

    vector<vector<shingle_hash>> tree; // the hash shingle tree

    map<size_t, string> dictionary; // TODO: transfer into index of the string to save auxilary space

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

    void update_tree(vector<size_t> hash_vector, size_t level);

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


};
#endif //CPISYNCLIB_SETSOFCONTENT_H
