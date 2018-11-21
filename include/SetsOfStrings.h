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
#include "CPI.h"
#include "SyncMethod.h"

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

    ~SetsOfStrings();

    void injectString(string str);


protected:
    bool oneway = true;

private:

    void encoding();

    void decoding();

    vector<K_Shingle> myKShingles; //shingle + be Burjin graph + backtracking
    size_t BlockSize, Levels, substrSize;
    const static size_t NOT_SET = 0; // size_t = unsigend int
};
#endif //CPISYNCLIB_SETSOFSTRINGS_H
