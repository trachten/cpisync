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

struct shingle_hash{
    size_t first, second, occurr;
};

//bool shingle_hash_com(shingle_hash a, shingle_hash b) { return a.first < b.first; };



class SetsOfContent : public SyncMethod {
public:
    SetsOfContent(size_t terminal_str_size = 50, size_t levels = NOT_SET);

    ~SetsOfContent() = default;

    void injectString(string str);


// functions for  SyncMethods
    bool addStr(DataObject* str, vector<DataObject*> &datum,  bool sync) override;

    string getName() override {return "Sets of Content";}
private:

    string myString; // original input string
    size_t TermStrSize, Levels, Partition;

    // each level: store string and their conter-part on the other side
    vector<map<size_t, size_t>> conformingPair;

    vector<vector<vector<shingle_hash>>> tree; // the hash shingle tree

    map<size_t, string> dictionary; // TODO: transfer into index of the string to save auxilary space

    size_t str_to_hash(string str) {
        return std::hash<std::string>{}(str);
    };

    /**
     * Create content dependent partitions based on the input string
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

    // Experiemntal function
    void update_tree(vector<size_t> hash_vector, size_t level);
};
#endif //CPISYNCLIB_SETSOFCONTENT_H
