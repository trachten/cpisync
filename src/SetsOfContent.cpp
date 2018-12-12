//
// Created by Bowen Song on 12/9/18.
//

#include "SetsOfContent.h"

SetsOfContent::SetsOfContent(size_t terminal_str_size, size_t levels)
: TermStrSize(terminal_str_size), Levels(levels){
    Partition = NOT_SET; // Set Them Later

}

vector<size_t> SetsOfContent::create_HashSet(string str,size_t win_size, size_t space, size_t shingle_size) {
    // space and shingle_size has to match between reconciling strings
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

void SetsOfContent::injectString(string str) {
    if (str.empty()) return;
    myString = str;
    // using Floor will let the terminal string tolerate up to 10 times of string size
    // should obey the rules of p^l >/< n/t , where p = partition, l = levels, n=string size, and t=terminal string size
    if(Levels==NOT_SET) {
        Partition = 10;
        Levels = floor((log2(str.size())-log2(TermStrSize))/log2(Partition));
    } else {
        Partition = floor(pow(2, (log2(str.size()) - log2(TermStrSize)) / Levels));
    }
    if (Levels==NOT_SET) throw invalid_argument("Consider set a Level value bigger than 0");

    size_t shingle_size = floor(log2(str.size()));
    size_t space = TermStrSize*126; //126 for ascii content

    // fill up the tree and dictionary
    tree.resize(Levels);
    auto first_level = create_HashSet(str,floor((str.size()/Partition)/2),space,shingle_size);
    update_tree(first_level,0);


}

void SetsOfContent::update_tree(vector<size_t> hash_vector, size_t level) {
    if (tree.size() <= level) throw invalid_argument("We have excceeded the levels of the tree");
    if (hash_vector.size() > 100) cout << "It is advised to not excceed 50 partitions for fast backtracking" << endl;
    map<pair<size_t,size_t>,size_t> tmp;
    for (int i = 0; i < hash_vector.size() - 1; ++i) {
        if(tmp[{hash_vector[i],hash_vector[i + 1]}])
            tmp[{hash_vector[i],hash_vector[i + 1]}]++;
        else
            tmp[{hash_vector[i],hash_vector[i + 1]}] = 1;
    }
    vector<shingle_hash> lst;
    for(auto item: tmp) lst.emplace_back(shingle_hash{.first = item.first.first, .second = item.first.second, .occurr = item.second});
//    sort(lst.begin(), lst.end(),shingle_hash_com); //order in lexicographical order for backtracking

    tree[level].push_back(lst); // put it in the tree
}


// functions for  SyncMethods
bool SetsOfContent::addStr(DataObject *str, vector<DataObject *> &datum, bool sync) {

    return true;
}







