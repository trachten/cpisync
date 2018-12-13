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

    // if the substring is smaller than the terminla stringh size, we do not partition it anymore.

    if (str.size()<=TermStrSize) return vector<size_t>({str_to_hash(str)});
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
    if (Levels == NOT_SET) {
        Partition = 10;
        Levels = floor((log2(str.size()) - log2(TermStrSize)) / log2(Partition));
    } else {
        Partition = floor(pow(2, (log2(str.size()) - log2(TermStrSize)) / Levels));
    }
    if (Levels == NOT_SET) throw invalid_argument("Consider set a Level value bigger than 0");

    size_t shingle_size = floor(log2(str.size()));
    size_t space = TermStrSize * 126; //126 for ascii content

    // fill up the tree and dictionary
    tree.resize(Levels);
    auto first_level = create_HashSet(str, floor((str.size() / Partition) / 2), space, shingle_size);
    update_tree(first_level, 0);

    for (int l = 1; l < Levels; ++l) {
        space = floor((space / Partition) / 2);
        shingle_size = floor(shingle_size / 2);
        for (auto substr_hash:unique_substr_hash(tree[l - 1])) {
            string substring = dictionary[substr_hash];
            auto tmp_level = create_HashSet(substring, floor((substring.size() / Partition) / 2),
                                            space, shingle_size);
            update_tree(tmp_level, l);
        }

    }
}

void SetsOfContent::update_tree(vector<size_t> hash_vector, size_t level) {
    if (tree.size() <= level) throw invalid_argument("We have excceeded the levels of the tree");
    if (hash_vector.size() > 100)
        cout
                << "It is advised to not excceed 50 partitions for fast backtracking at Level: " + to_string(level) +
                   " Current set size: " + to_string(hash_vector.size()) << endl;
    if (hash_vector.empty())
        throw invalid_argument("hash_vector is zero at level:" + to_string(level) +
                               ". All termianl strgins to be passed down to the bottom level");

    map<pair<size_t, size_t>, size_t> tmp;
    for (int i = 0; i < hash_vector.size() - 1; ++i) {
        if (tmp[{hash_vector[i], hash_vector[i + 1]}])
            tmp[{hash_vector[i], hash_vector[i + 1]}]++;
        else
            tmp[{hash_vector[i], hash_vector[i + 1]}] = 1;
    }

    size_t group_id = get_group_signature(hash_vector);

    // incase a string was not partitioned based on the given window and space size, (either, string is small enough, or it would be partitioned again in a subsequent level)
    if (tmp.empty()) {
        // passs down termianl string, if not partitioned
        if (hash_vector.empty()) throw invalid_argument("inherited empty hash vector at level: " + to_string(level));
        tree[level].emplace_back(
                shingle_hash{.first = hash_vector.back(), .second = 0, .occurr = 1, .groupID = group_id, .cycleVal = 1});
        return;
    }

    vector<shingle_hash> lst;

    auto cyc = get_cyc_val(hash_vector);

    auto item = tmp.begin();
    lst.emplace_back(
            shingle_hash{.first = item->first.first, .second = item->first.second, .groupID = group_id, .occurr = item->second, .cycleVal = cyc});
    std::advance(item, 1);
    for (; item != tmp.end(); ++item) {
        lst.emplace_back(
                shingle_hash{.first = item->first.first, .second = item->first.second, .groupID = group_id, .occurr = item->second, .cycleVal = 0});
    }

    sort(lst.begin(), lst.end()); // sorting might be unnecessary here due to the use of hashmap above
    //order in lexicographical order for backtracking (sort once at insert, sort another time at reconstruction)

    tree[level].insert(tree[level].end(), lst.begin(), lst.end()); // put it in the tree
}

size_t SetsOfContent::get_group_signature(vector<size_t> unsorted_hashset) {
    // cover one element situation as well
    // make sure unique
    // make usre it can just be 0 by chance which is not going to be unique
}

multiset<string> SetsOfContent::getTerminalStr() {
    vector<string> terminal_str;

    for (size_t hash_item: unique_substr_hash(tree.back())) {
        terminal_str.push_back(dictionary[hash_item]);
    }

    return multiset<string>(terminal_str.begin(), terminal_str.end());
}


// functions for backtracking

void SetsOfContent::shingle2hash_train(vector<shingle_hash> shingle_set, size_t &str_order,
                                       vector<size_t> &final_str) {
    // nxtEdgeStack: [[idx of nxt edges];[];...] Register the state of nxt possible edges
    // stateStack: [[occr of shingles ];[];...] Register the state of shigle set occurrences
    vector<vector<size_t>> nxtEdgeStack, stateStack;

}




// functions for  SyncMethods
bool SetsOfContent::addStr(DataObject *str, vector<DataObject *> &datum, bool sync) {

    return true;
}







