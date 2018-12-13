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
                shingle_hash{.first = hash_vector.back(), .second = 0, .occurr = 1, .groupID = group_id, .cycleVal = 1, .lvl = level});
        return;
    }

    vector<shingle_hash> lst;

    for (auto item = tmp.begin(); item != tmp.end(); ++item) {
        lst.emplace_back(
                shingle_hash{.first = item->first.first, .second = item->first.second, .groupID = group_id, .occurr = item->second, .cycleVal = 0, .lvl = level});
    }

    auto cyc = set_cyc_val(lst,hash_vector); // set the cycle number at the head shingle


//    sort(lst.begin(), lst.end()); // sorting might be unnecessary here due to the use of hashmap above
    //order in lexicographical order for backtracking (sort once at insert, sort another time at reconstruction)

    tree[level].insert(tree[level].end(), lst.begin(), lst.end()); // put it in the tree
}

size_t SetsOfContent::get_group_signature(vector<size_t> strordered_hashset) {
    // cover one element situation as well
    // make sure unique
    // make usre it can just be 0 by chance which is not going to be unique
    return strordered_hashset.front();
}

multiset<string> SetsOfContent::getTerminalStr() {
    vector<string> terminal_str;

    for (size_t hash_item: unique_substr_hash(tree.back())) {
        terminal_str.push_back(dictionary[hash_item]);
    }

    return multiset<string>(terminal_str.begin(), terminal_str.end());
}


// functions for backtracking
bool SetsOfContent::shingle2hash_train(vector<shingle_hash> shingle_set, size_t &str_order,
                                                                    vector<size_t>& final_str) {
    // edge case if there is only one shinlge in the set
    if (shingle_set.empty()) throw invalid_argument("Nothing is passed into shingle2hash_train");

    if (shingle_set.size() == 1) {// edge case of just one shingle
        if (str_order == 0) {// we find cycle number
            str_order = 1;
            return true;
        } else { // we find string
            return true;
        }
    }



    // nxtEdgeStack: [[idx of nxt edges];[];...] Register the state of nxt possible edges
    // stateStack: [[occr of shingles ];[];...] Register the state of shigle set occurrences
    vector<vector<size_t>> nxtEdgeStack;
    vector<vector<shingle_hash>> stateStack;
    size_t curEdge, strCollect_size = 0;
    vector<size_t> str; // trmprary string hash train to last be compared/placed in final_str

    if (str_order == 0) { // find head from "final_str" (we are finding cycle number)
        for (shingle_hash &shingle : shingle_set) {
            if (shingle.first == final_str[0] and shingle.second == final_str[1]) {
                shingle.occurr--;
                str.push_back(shingle.first);
                str.push_back(shingle.second);
                curEdge = shingle.second;
                break;
            }
        }
    } else {// else find head is already inserted (we are retrieving string)
        curEdge = final_str[1];
    }


    // Init Original state
    stateStack.push_back(shingle_set);
    size_t nxt_idx = 0;
    while (!stateStack.empty() and stateStack.size() == nxtEdgeStack.size() + 1) { // while state stack is not empty
        vector<size_t> nxtEdges = get_nxt_edge_idx(curEdge, stateStack.back());

        if (!nxtEdges.empty()) { // If we can go further with this route
            nxt_idx = nxtEdges.back();
            nxtEdges.pop_back();
            nxtEdgeStack.push_back(nxtEdges);
        } else if (!nxtEdgeStack.empty() and stateStack.size() == nxtEdgeStack.size() + 1 and
                   !nxtEdgeStack.back().empty()) { //if this route is dead, we should look for other options
            if (!str.empty()) str.pop_back();

            //look for other edge options
            nxt_idx = nxtEdgeStack.back().back();
            nxtEdgeStack.back().pop_back();

            stateStack.pop_back();
            //(!stateStack.empty()) ? stateStack.push_back(stateStack.back()) : stateStack.push_back(origiState);
        } else if (!stateStack.empty() and stateStack.size() == nxtEdgeStack.size() + 1 and
                   nxtEdgeStack.back().empty()) {// if this state is dead and we should look back a state
            if (!str.empty()) str.pop_back();
            // look back a state or multiple if we have empty nxt choice (unique nxt edge)
            while (!nxtEdgeStack.empty() and nxtEdgeStack.back().empty()) {
                nxtEdgeStack.pop_back();
                stateStack.pop_back();
                if (!str.empty()) str.pop_back();
            }
            if (nxtEdgeStack.empty()) {
                return false;
            } else if (!nxtEdgeStack.back().empty()) {
                nxt_idx = nxtEdgeStack.back().back();
                nxtEdgeStack.back().pop_back();
                stateStack.pop_back();
            }
        } else if (stateStack.size() != nxtEdgeStack.size() + 1) {
            throw invalid_argument("state stack and nxtEdge Stack size miss match" + to_string(stateStack.size())
                                   + ":" + to_string(nxtEdgeStack.size()));
        }

        str.push_back(shingle_set[nxt_idx].second);

        // Change and register our state for shingle occurrence and nxt edges
        stateStack.push_back(stateStack.back());
        stateStack.back()[nxt_idx].occurr--;

        curEdge = shingle_set[nxt_idx].second;

        // if we reached a stop point
        if (empty_state(stateStack.back())) {
            strCollect_size++;
            if (str == final_str || (strCollect_size == str_order and str_order != 0)) {
                str_order = strCollect_size;
                final_str = str;
            }
        }

        if (strCollect_size == str_order && str_order != 0) {
            return true;
        }
    }
    return false;
}

bool SetsOfContent::empty_state(vector<shingle_hash> state) {
    for (shingle_hash item : state) {
        if (item.occurr > 0) return false;
    }
    return true;
}

vector<size_t> SetsOfContent::get_nxt_edge_idx(size_t current_edge, vector<shingle_hash> changed_shingleOccur) {
    vector<size_t> tmplst;
    for (size_t i = 0; i < changed_shingleOccur.size(); ++i){
        if (changed_shingleOccur[i].first == current_edge && changed_shingleOccur[i].occurr > 0){
            tmplst.push_back(i);
        }
    }
    return tmplst;
}

size_t SetsOfContent::set_cyc_val(vector<shingle_hash> &shingle_set,
                                          vector<size_t> strordered_substr_hash) {
    size_t cycnum = 0;

    shingle2hash_train(shingle_set,cycnum,strordered_substr_hash); // get the cycnum

    for(shingle_hash& shingle : shingle_set){
        if(shingle.first==strordered_substr_hash[0] and shingle.second==strordered_substr_hash[1]){
            shingle.cycleVal = cycnum; // put in the head
        }
    }
    return cycnum;
}

string SetsOfContent::get_str_from(vector<shingle_hash> shingle_set) {
    vector<size_t> final_str;
    string recovered_str;
    for (shingle_hash& shingle : shingle_set){
        if (shingle.cycleVal>0) { // this means it is the head
            shingle.occurr--;
            final_str.push_back(shingle.first);
            final_str.push_back(shingle.second);
            shingle2hash_train(shingle_set,shingle.cycleVal,final_str);
            break; // there should not be any other shingles that has it
        }
    }
    for (size_t substr_hash : final_str){
        recovered_str+=dictionary[substr_hash];
    }
    return recovered_str;
}

vector<string> SetsOfContent::get_all_strs_from(vector<shingle_hash> level_shingle_set) {
    // assume strings are from the same level
    vector<string> res;
    map<size_t,vector<shingle_hash>> shingle_group;
    for(shingle_hash shingle:level_shingle_set){
        // report none-unique group ID, there should no be any, since only the unique strings get expanded, If none unique, it is the group ID
        shingle_group[shingle.groupID].push_back(shingle);
    }
    // update dictionary
    for(auto group:shingle_group){
        res.push_back(get_str_from(group.second));
        add_to_dictionary(res.back());
    }
    return res;
}

string SetsOfContent::retriveString() {
    for (int i = tree.size()-1; i > 0; --i){
        get_all_strs_from(tree[i]);
    }
    return get_all_strs_from(tree[0]).back();
}
// functions for  SyncMethods
bool SetsOfContent::addStr(DataObject *str, vector<DataObject *> &datum, bool sync) {

    return true;
}







