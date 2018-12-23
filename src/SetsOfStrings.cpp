//
// Created by Bowen Song on 11/20/18.
//

#include "SetsOfStrings.h"

SetsOfStrings::SetsOfStrings(const size_t block_size, size_t substr_size, size_t levels, const size_t shingle_size,
        const char stop_word) : BlockSize(block_size), shingleSize(shingle_size), stopWord(stop_word) {

    // num of levels and block size same guarantees same number of terminal blocks
    if (levels == NOT_SET) { // Either depend on substring size
        substrSize = substr_size;
        Levels = NOT_SET; // set later in injectString
        throw invalid_argument("SetsOfStrings does not support defining only the sub str size yet");
// TODO: set levels and block size parameters according to the authoritarian string size, control terminal string size to a small constant
    } else { // Or depend on number of levels
        substrSize = NOT_SET;
        Levels = levels;
    }
}

void SetsOfStrings::injectString(string str) {
    if (Levels == NOT_SET) Levels = ceil((str.size() / substrSize) / BlockSize);
    // TODO: ensure final substring size inside bound
//    if (substrSize == NOT_SET) substrSize = floor((str.size()/Levels)/BlockSize);
    myString = str;
    encoding();
}

void SetsOfStrings::encoding() {
    if (Levels <= NOT_SET or myString.empty()) //sanity check
        throw invalid_argument("No string input or Levels is not > 0, Levels: " + to_string(Levels));
//parameters
    vector<vector<string>> str_par(Levels); // init str_par
    vector<string> firslevel;

    //Content-dependent parameters
    idx_t shingle_size = log10(myString.size());// consider how long of a content
    idx_t space = shingle_size*126; // for ascii racaters,
    clock_t begin = clock();
    contentDepParition(myString, space,shingle_size, 500,firslevel); // create first level
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    str_par[0].insert(str_par[0].begin(), firslevel.begin(), firslevel.end()); // append into first level
    for (int i = 1; i < Levels; ++i) {
        for (const auto level_str : str_par[i - 1]) {
            vector<string> templvl;
            contentDepParition(level_str,space,shingle_size, 500,templvl); // create sub_substrings per substring
            str_par[i].insert(str_par[i].begin(), templvl.begin(),
                              templvl.end()); // append into one vector at every level
        }
    }
    subStrSet = str_par;
    // TODO: figure out if keeping the num of terminal string same is important

    // TODO: calculate permutation order for every substring -> transmit to the other side
}



//StrataEst SetsOfStrings::getStrataEst() {
//    K_Shingle my_shingle = K_Shingle(shingleSize, stopWord);
//    my_shingle.inject(myString);
//    vector<string> shingle_set = my_shingle.getShingleSet_str();
//    for(string shingle : shingle_set){
//        myStrata.insert(new DataObject());
//    }
//    return myStrata;
//}
//
//size_t SetsOfStrings::setDiffUpperBound(StrataEst theirStrata) {
//    return (theirStrata-=myStrata).estimate();
//}


size_t SetsOfStrings::create_substrings(const string str, vector<pair<string,size_t>> & substr_pairset) {
    size_t order = 0; // permutation order
    if (BlockSize > NOT_SET) {
        vector<string> substr(BlockSize);
        for (size_t i = 0; i < str.size(); ++i) {
            substr[((i + 1) % BlockSize)] += str[i];
        }

        map<string, size_t> tempset;
        for (string str : substr){
            (tempset[str])? tempset[str]+=1: tempset[str] = 1;
        }
        for (auto it = tempset.begin(); it != tempset.end(); ++it) substr_pairset.emplace_back(it->first,it->second);

    } else {
        throw invalid_argument("Block Size should be bigger than 0, instead of current value: " + to_string(BlockSize));
    }

    return 0; // a set of hashes
}



// Content-dependent partitioning
size_t SetsOfStrings::contentDepParition(string str, idx_t space, idx_t shingle_size, idx_t win_size, vector<string> &substr_pairset) {
    vector<idx_t> hash_val;
    for (idx_t i = 0; i < str.size() - shingle_size + 1; ++i) {
        std::hash<std::string> shash;
        hash_val.push_back(shash(str.substr(i, shingle_size)) % space);
    }
    idx_t prev = 0;
    idx_t last_min = space; // Biggest number is the space
    for (idx_t j = win_size; j < hash_val.size() - win_size; ++j) {
        if (hash_val[j] == min_between(hash_val, (j - win_size), j + win_size) )
//        and (contains_between(hash_val, j - win_size, j - 1, hash_val[j]) or
//             contains_between(hash_val, j + 1, j + win_size, hash_val[j])))
//        if ((last_min==hash_val[j] and hash_val[j-win_size] != last_min and min(last_min,hash_val[j+win_size])) or
//                hash_val[j] == min_between(hash_val, (j - win_size), j + win_size) )
        {
            substr_pairset.push_back(str.substr(prev, j - prev));
            prev = j;
        }

    }
    substr_pairset.push_back(str.substr(prev));
    return 0; // TODO: return permutaiton order
}

// Concept testing

multiset<string> SetsOfStrings::getTerminalSubstr() {
    multiset<string> res;
    for(auto subpair : subStrSet.back()) {
        res.emplace(subpair);
//        K_Shingle tmp = K_Shingle(4);
//        tmp.inject(subpair);
//        for( auto item :tmp.getShingleSet())
//            res.insert(item);
    }
    return res;
}