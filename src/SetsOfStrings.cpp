//
// Created by Bowen Song on 11/20/18.
//

#include "SetsOfStrings.h"

SetsOfStrings::SetsOfStrings(const size_t block_size, size_t substr_size, size_t levels, const size_t shingle_size,
        const char stop_word) {
    BlockSize = block_size;

    if (levels == NOT_SET) { // Either depend on substring size
        substrSize = substr_size;
    } else { // Or depend on number of levels
        substrSize = NOT_SET;
        Levels = levels;
    }
}