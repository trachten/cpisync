//
// Created by Bowen Song on 12/9/18.
//

#include "SetsOfContent.h"

SetsOfContent::SetsOfContent(size_t block_size, size_t terminal_str_size, size_t levels)
: BlockSize(block_size), TermStrSize(terminal_str_size), Levels(levels){

}

vector<size_t> SetsOfContent::create_HashSet(string str, size_t space, size_t shingle_size, size_t win_size) {
    vector<size_t> hash_val, hash_set;
    for (size_t i = 0; i < str.size() - shingle_size + 1; ++i) {
        std::hash<std::string> shash;
        hash_val.push_back(shash(str.substr(i, shingle_size)) % space);
    }
    size_t prev = 0;
    size_t last_min = space; // Biggest number is the space
    for (size_t j = win_size; j < hash_val.size() - win_size; ++j) {
        if (hash_val[j] == min_between(hash_val, (j - win_size), j + win_size) )
        {
            hash_set.push_back(add_to_dictionary(str.substr(prev, j - prev)));
            prev = j;
        }

    }
    hash_set.push_back(add_to_dictionary(str.substr(prev)));
    return hash_set;
}

size_t SetsOfContent::add_to_dictionary(string str) {
    (dictionary[str_to_hash(str)].empty() or dictionary[str_to_hash(str)] == str) ? dictionary[str_to_hash(str)] = str
                                                                                  : throw invalid_argument(
            "Dictionary duplicated suggest using new/multiple hashfunctions");
    return str_to_hash(str);
}

bool SetsOfContent::addStr(DataObject *str, vector<DataObject *> &datum, bool sync) {
    size_t shingle_size = log10(myString.size());// consider how long of a content
    size_t space = shingle_size*126; //126 for ascii content

}


// TODO: partition to small chunks and have one number to reprensent each chunck.