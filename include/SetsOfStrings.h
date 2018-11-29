//
// Created by Bowen Song on 11/19/18.
//
//

#ifndef CPISYNCLIB_SETSOFSTRINGS_H
#define CPISYNCLIB_SETSOFSTRINGS_H

#include <vector>
#include <string>
#include <NTL/ZZ.h>
#include "kshingling.h"
#include "StrataEst.h"
#include "CPI.h"
#include "SyncMethod.h"
#include "DataObject.h"

using std::vector;
using std::hash;
using std::string;
using namespace NTL;
typedef unsigned long idx_t;

class SetsOfStrings {
public:
    /**
     * Constructor: based on blocksize and number of levels
     * @param block_size number of characters for each block | fixed to achieve constant time for permutation ordering
     * @param substr_size length of the terminal substrings | controls K_shingling comm and time cost
     * @param levels number of levels for sets of sets | exponent on the block size
     */
    SetsOfStrings(size_t block_size = 5, size_t substr_size = 500, size_t levels = NOT_SET, size_t shingle_size = 9,
                  char stop_word = '$');

    ~SetsOfStrings() = default;

    void injectString(string str);

    /**
     * get a Strata Estimator for the entire string using based form kshingle parameters
     * @return Strata Estimator to compare with reconcialing set
     * Important for configuring base sync parameters
     */
    StrataEst getStrataEst();

    /**
     * Get upper bound on the string difference, used to configure base sync parameters
     * @param theirStrata
     * @return
     */
    size_t setDiffUpperBound(StrataEst theirStrata);

    /**
     * get 2D K_Shingle: substring
     * @return
     */
    vector<vector<K_Shingle>> getShingleSet();

    // Concept testing
    multiset<pair<string,size_t>> getTerminalSubstr();

    multiset<pair<string,size_t>> setTotoalKshingleset();



protected:
    bool oneway = true;

private:

    void encoding();

    void decoding();

    /**
     * Split a string by a block size and each character to a slot of partitions, num of partition == block size.
     * @param str string to be split
     * @param substr_pairset substring and occurrence pair
     * @return a set of hashes TODO: change it to a bunch of hashes
     */
    size_t create_substrings(string str, vector<pair<string,size_t>> & substr_pairset);


    // Sets of strings parameters
    vector<vector<pair<string,size_t>>> subStrSet;
    vector<vector<string>> confromSet;

    string myString; // original input string
    size_t BlockSize, Levels, substrSize;
    const static size_t NOT_SET = 0; // size_t = unsigend int | dependent value 0: e.g. BlockSize > NOT_SET

    // Kshingling Parameters
    vector<K_Shingle> myKShingles; //shingle + be Burjin graph + backtracking
    char stopWord;
    size_t shingleSize;

};
#endif //CPISYNCLIB_SETSOFSTRINGS_H
