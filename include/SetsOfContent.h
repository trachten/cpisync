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

typedef pair<pair<size_t, size_t>, size_t> shingle_hash; //pair<pair<first_str_hash,second_str_hash>,occurrence>

class SetsOfContent : public SyncMethod {
public:
    SetsOfContent(size_t terminal_str_size = 50, size_t levels = NOT_SET);

    ~SetsOfContent() = default;

    void injectString(string str);

    bool addStr(DataObject* str, vector<DataObject*> &datum,  bool sync) override;

    string getName() override {return "Sets of Content";}
private:

    string myString; // original input string
    size_t TermStrSize, Levels;

    // each level: store string and their conter-part on the other side
    vector<map<size_t, size_t>> conformingPair;

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
    vector<size_t> create_HashSet(string str, size_t space, size_t shingle_size, size_t win_size);

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
};
#endif //CPISYNCLIB_SETSOFCONTENT_H
