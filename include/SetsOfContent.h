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
    size_t first, second, occurr, groupID;
    unsigned short cycleVal;
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


// functions for  SyncMethods
    bool addStr(DataObject* str, vector<DataObject*> &datum,  bool sync) override;

    string getName() override {return "Sets of Content";}

    /**
     * Get the terminal strings to reconcile with another set
     * @return a vector of terminla string
     */
    multiset<string> getTerminalStr();

    //getShinglesAt
    multiset<size_t> getShinglesSumAt(int lvl) {
        if (lvl < 0 or lvl >= tree.size())
            throw invalid_argument("getShinglesAt a lvl: " + to_string(lvl) + "is out of the range of the tree");
//        return multiset<shingle_hash>(tree[lvl].begin(), tree[lvl].end());
        multiset<size_t> res;
        for(auto item:tree[lvl]){
            res.insert(item.first+item.occurr+item.second+item.groupID);
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
     * @param str input string
     * @param space
     * @param shingle_size
     * @param win_size
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

    // extractr the unique substring hashes from the shingle_hash vector
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

    size_t get_group_signature(vector<size_t> unsorted_hashset);

    // functions for backtracking
    /**
     * peice shingle_hashes back to a vector of hashes in the string order
     * @param shingle_set
     * @param str_order
     * @param final_str a hash train in string order
     */
    void shingle2hash_train(vector<shingle_hash> shingle_set,unsigned short &str_order, vector<size_t> &final_str);

    unsigned short get_cyc_val(vector<shingle_hash> shingle_set, vector<size_t> final_str);

    string get_str_from(vector<shingle_hash> shingle_set, unsigned short str_order);


};
#endif //CPISYNCLIB_SETSOFCONTENT_H
