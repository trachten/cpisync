//
// Created by Bowen on 9/18/18.
// Break string into a fixed length set of shingles
// reconstruct a possible string from shingle set
// calculate if that is the only sting available
//

#include "kshingling.h"


K_Shingle::~K_Shingle() = default;

K_Shingle::K_Shingle(const size_t shingle_size, const char stop_word)
: k(shingle_size), stopword(stop_word) {
    virtualMemMonitor(initVM);
}

bool K_Shingle::create(const string str) {
    //  Sanity check
    //for (int i = 0; i < str.size(); ++i) {
    if (str.find(stopword) != string::npos) {
        throw invalid_argument("Input string includes Stopword");
        //return false; // either log or invalid
    }
    //}
    if (k > orig_string.size()) {
        throw invalid_argument("Shingle size has to be smaller than the string length");
        //return false;
    } else if (k < 2) {
        throw invalid_argument("Minimum shingle size has to be bigger than 2");
        //return false;
    } else if (!orig_string.empty()) {
        //create a set of shingle in shingleSet
        // first do it in map, then vector
        map<string, idx_t> tmpShingleMap;
        for (int i = 0; i < orig_string.size() - k + 1; ++i) {
            string nxt_Shingle = orig_string.substr(i, k);
            incrementEdgeCount(nxt_Shingle, tmpShingleMap);
        }
        for (auto item : tmpShingleMap){
            shingleSet.emplace_back(item.first, item.second);
            shingleSet_str.push_back(item.first+":"+to_string(item.second));
        }
    } else {
        throw invalid_argument("No input string");
        //return false;
    }

    return true;
}



void K_Shingle::incrementEdgeCount(const string ver, map<string,idx_t> & shingle_map) {
    if (!ver.empty()) {

        auto it = shingle_map.find(ver);
        (it != shingle_map.end()) ? shingle_map[ver] += 1 : shingle_map[ver] = 1;

    } else {
        throw invalid_argument("No string in vertex");
    }
}


vector<idx_t>  K_Shingle::getEdgeIdx(const string verStart, vector<idx_t> changed_shingleOccur) {
    if (!verStart.empty()) { // verStart not empty
        vector<idx_t> templst;
        // sorted list, start to find it then, as soon as the next is not starting with the substring, bail.
        for (idx_t i = 0; i < shingleSet.size(); ++i) {
            if (shingleSet[i].first.substr(0,verStart.size()) == verStart && changed_shingleOccur[i]>0){
                templst.push_back(i);
            }
        }
        return templst; // no hit unless last element is included in the list
    } else {
        throw invalid_argument("No vertex start string for searching");
    }
}

pair<string,idx_t> K_Shingle::reconstructStringBacktracking(idx_t strOrder) {
    idx_t strCollect_size = 0;
    string startString;


    //sort it in lexicographic order
    sort(shingleSet.begin(), shingleSet.end());
    auto changed_shingleSet = shingleSet;

    // find the head
    for (auto it = changed_shingleSet.begin(); it != changed_shingleSet.end(); ++it) {
        if (it->first[0] == stopword) {
            startString = it->first;
            it->second--;
            break;
        }
    }
// Get the string or string cycle number
    if (!startString.empty()) {
//        // Delete the first edge by value
        string final_str;
        shingle2string(changed_shingleSet, startString, strCollect_size, strOrder, final_str, startString);
        if (strCollect_size == 0 || strCollect_size < strOrder) { // failed to recover a string
            return make_pair("", 0);  // return 0 for fail
        }
        return make_pair(final_str, strOrder);
    } else {
        throw invalid_argument("Shingle Set does not have a start point");
    }
}


bool K_Shingle::shingle2string(vector<pair<string,idx_t>> changed_shingleOccur, string curEdge, idx_t &strCollect_size,
                               idx_t &str_order, string &final_str, string str) {

/**
 * nxtEdgeStack: [[idx of nxt edges];[];...] Register the state of nxt possible edges 
 * stateStack: [[occr of shingles ];[];...] Register the state of shigle set occurrences
 */
    vector<vector<idx_t>> nxtEdgeStack, stateStack; // check and can not be negative

    vector<idx_t> origiState(changed_shingleOccur.size()); // compose the original state
    for (idx_t i = 0; i < changed_shingleOccur.size(); ++i) {
        origiState[i] = changed_shingleOccur[i].second;
    }


    // Init Original state
    stateStack.push_back(origiState);
    idx_t nxt_idx = 0;

    while (!stateStack.empty() and stateStack.size() == nxtEdgeStack.size() + 1) { // while state stack is not empty

        if (!virtualMemMonitor(initVM)) return false;
//        printMemUsage();
        auto nxtEdges = getEdgeIdx(curEdge.substr(1), stateStack.back());

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



        str += shingleSet[nxt_idx].first.back();

        // Change and register our state for shingle occurrence and nxt edges
        stateStack.push_back(stateStack.back());
        stateStack.back()[nxt_idx] -= 1;

        curEdge = shingleSet[nxt_idx].first;

        // if we reached a stop point
        if (shingleSet[nxt_idx].first.back() == stopword and emptyState(stateStack.back())) {
            strCollect_size++;
            if (str == orig_string || (strCollect_size == str_order and str_order != 0)) {
                str_order = strCollect_size;
                final_str = str.substr(1, str.size() - 2);
            }
        }



        if (strCollect_size == str_order && str_order != 0) {
            UpdateUsedVM();
            return true;
        }
    }
    return false;
}


void K_Shingle::updateShingleSet_str(string shingle) {
    try {
        int ind = shingle.find_last_of(":");
        shingleSet.emplace_back(shingle.substr(0, ind), stoi(shingle.substr(ind + 1)));
    }
    catch (std::exception e) {
        cout << "what do we have here: " << shingle << endl;
    }
    shingleSet_str.push_back(shingle);
}

//void K_Shingle::shingle2string_recursion(vector<pair<string,int>> changed_shingleSet, string curEdge, int &strCollect_ind,int &str_order,string &finalstr, string str) {
//
//    auto lst = getEdges(curEdge.substr(1), changed_shingleSet);
//    for (auto it = lst.begin(); it != lst.end(); ++it) {
//        pair<string, int> tempedge = *it;
//        str += tempedge.first.substr(k - 1);
//
//        if (it->second > 1) {
//            it->second--;
//            find(changed_shingleSet.begin(), changed_shingleSet.end(), tempedge)->second--;
//        } else {
//            //lst.erase(it);
//            changed_shingleSet.erase(remove(changed_shingleSet.begin(), changed_shingleSet.end(), tempedge), changed_shingleSet.end());
//        }
//        if (tempedge.first.substr(k - 1) == stopword) {
//            if (changed_shingleSet.size() == 0) {
//                strCollect_ind++;
//                if (str == orig_string || strCollect_ind - 1 == str_order) {
//                    str_order = strCollect_ind - 1;
//                    finalstr = str.substr(1,str.size()-2);
//                }
//            }
//
//        }
//        shingle2string(changed_shingleSet, tempedge.first, strCollect_ind, str_order, finalstr, str);
//        if (strCollect_ind-1==str_order && str_order!=-1){
//            return;
//        }
//        if (tempedge.second > 1) {
//            it->second++;
//            find(changed_shingleSet.begin(), changed_shingleSet.end(), tempedge)->second++;
//        } else {
//            //lst.push_back(*it);
//            changed_shingleSet.push_back(tempedge);
//        }
//        str.pop_back();
//    }
//}
